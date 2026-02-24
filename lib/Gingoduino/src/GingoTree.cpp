// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoTree.
//
// SPDX-License-Identifier: MIT

#include "GingoTree.h"

#if GINGODUINO_HAS_TREE

#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GingoTree::GingoTree(const char* tonic, ScaleType type, uint8_t traditionId)
    : field_(tonic, type)
    , traditionId_(traditionId < PROG_TRADITION_COUNT ? traditionId : 0)
{
    // Determine context from scale type
    ScaleType st = field_.scale().parent();
    ctx_ = (st == SCALE_NATURAL_MINOR || st == SCALE_HARMONIC_MINOR ||
            st == SCALE_MELODIC_MINOR)
         ? PROG_CTX_MINOR : PROG_CTX_MAJOR;
}

// ---------------------------------------------------------------------------
// Branch lookup
// ---------------------------------------------------------------------------

uint8_t GingoTree::findBranch(const char* name) {
    if (!name) return 0xFF;
    for (uint8_t i = 0; i < PROG_BRANCH_COUNT; i++) {
        const char* ptr = (const char*)pgm_read_ptr(&data::PROG_BRANCH_NAMES[i]);
        char buf[24];
        data::readPgmStr(buf, ptr, sizeof(buf));
        if (strcmp(name, buf) == 0) return i;
    }
    return 0xFF;
}

// ---------------------------------------------------------------------------
// Edge lookup
// ---------------------------------------------------------------------------

bool GingoTree::hasEdge(uint8_t originId, uint8_t targetId) const {
    // Read edge table pointer and count from PROGMEM
    const data::ProgEdge* edges;
    uint8_t edgeCount;

    // Read the ProgEdgeTable struct from PROGMEM
    const data::ProgEdgeTable* tablePtr = &data::PROG_EDGE_TABLES[traditionId_][ctx_];
    edges = (const data::ProgEdge*)pgm_read_ptr(&tablePtr->edges);
    edgeCount = pgm_read_byte(&tablePtr->count);

    for (uint8_t i = 0; i < edgeCount; i++) {
        uint8_t o = pgm_read_byte(&edges[i].origin);
        uint8_t t = pgm_read_byte(&edges[i].target);
        if (o == originId && t == targetId) return true;
    }
    return false;
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

bool GingoTree::isValid(const char* origin, const char* target) const {
    uint8_t oid = findBranch(origin);
    uint8_t tid = findBranch(target);
    if (oid == 0xFF || tid == 0xFF) return false;
    return hasEdge(oid, tid);
}

bool GingoTree::isValidSequence(const char* const* sequence, uint8_t count) const {
    if (count < 2) return true;
    for (uint8_t i = 0; i + 1 < count; i++) {
        if (!isValid(sequence[i], sequence[i + 1])) return false;
    }
    return true;
}

uint8_t GingoTree::countValidTransitions(const char* const* sequence, uint8_t count) const {
    if (count < 2) return 0;
    uint8_t valid = 0;
    for (uint8_t i = 0; i + 1 < count; i++) {
        if (isValid(sequence[i], sequence[i + 1])) valid++;
    }
    return valid;
}

uint8_t GingoTree::neighbors(const char* branch, const char** output, uint8_t maxNeighbors) const {
    uint8_t branchId = findBranch(branch);
    if (branchId == 0xFF) return 0;

    const data::ProgEdgeTable* tablePtr = &data::PROG_EDGE_TABLES[traditionId_][ctx_];
    const data::ProgEdge* edges = (const data::ProgEdge*)pgm_read_ptr(&tablePtr->edges);
    uint8_t edgeCount = pgm_read_byte(&tablePtr->count);

    uint8_t written = 0;
    for (uint8_t i = 0; i < edgeCount && written < maxNeighbors; i++) {
        uint8_t o = pgm_read_byte(&edges[i].origin);
        if (o == branchId) {
            uint8_t t = pgm_read_byte(&edges[i].target);
            if (t < PROG_BRANCH_COUNT) {
                output[written++] = (const char*)pgm_read_ptr(&data::PROG_BRANCH_NAMES[t]);
            }
        }
    }
    return written;
}

// ---------------------------------------------------------------------------
// Resolve branch to concrete chord
// ---------------------------------------------------------------------------

bool GingoTree::resolve(const char* branch, char* chordName, uint8_t maxLen) const {
    if (!branch || maxLen < 4) return false;

    uint8_t bid = findBranch(branch);
    if (bid == 0xFF || bid >= PROG_BRANCH_COUNT) return false;

    // Read degree info
    uint8_t degree = pgm_read_byte(&data::BRANCH_DEGREE[bid].degree);
    uint8_t isSeventh = pgm_read_byte(&data::BRANCH_DEGREE[bid].isSeventh);

    // Direct degree mapping
    if (degree != 0xFF && degree >= 1 && degree <= 7) {
        GingoChord c = isSeventh ? field_.seventh(degree) : field_.chord(degree);
        const char* n = c.name();
        uint8_t i = 0;
        while (n[i] && i < maxLen - 1) { chordName[i] = n[i]; i++; }
        chordName[i] = '\0';
        return true;
    }

    // Special cases: parse the branch name for resolution

    // Secondary dominants: "V7 / X", "V / X", "II / X"
    // Find " / " separator
    const char* sep = strstr(branch, " / ");
    if (sep) {
        // Target is after " / "
        const char* targetBranch = sep + 3;
        // Find the target degree
        uint8_t targetBid = findBranch(targetBranch);
        if (targetBid != 0xFF && targetBid < PROG_BRANCH_COUNT) {
            uint8_t tDeg = pgm_read_byte(&data::BRANCH_DEGREE[targetBid].degree);
            if (tDeg != 0xFF && tDeg >= 1 && tDeg <= 7) {
                // Get the target chord's root
                GingoChord targetChord = field_.chord(tDeg);
                GingoNote targetRoot = targetChord.root();

                // "V7 / X" → dominant seventh of X → root is P5 above target
                // "V / X"  → dominant of X
                // "II / X" → supertonic of X
                if (strncmp(branch, "V7", 2) == 0) {
                    GingoNote domRoot = targetRoot.transpose(7); // P5 above = V of target
                    const char* rn = domRoot.name();
                    uint8_t i = 0;
                    while (rn[i] && i < maxLen - 2) { chordName[i] = rn[i]; i++; }
                    chordName[i++] = '7';
                    chordName[i] = '\0';
                    return true;
                }
                if (strncmp(branch, "V ", 2) == 0 || strcmp(branch + (sep - branch - 1), "V") == 0) {
                    GingoNote domRoot = targetRoot.transpose(7);
                    const char* rn = domRoot.name();
                    uint8_t i = 0;
                    while (rn[i] && i < maxLen - 2) { chordName[i] = rn[i]; i++; }
                    chordName[i++] = 'M';
                    chordName[i] = '\0';
                    return true;
                }
            }
        }
        // Compound branches like "IIm / IV" — resolve as the first part
        // "IIm / IV" → IIm of IV → the ii chord of the IV key
        // Simplified: just resolve the first part's degree
        // Parse roman numeral from prefix
    }

    // SUBV7: tritone substitution of V7
    if (strncmp(branch, "SUBV7", 5) == 0) {
        GingoChord v7 = field_.seventh(5);
        GingoChord subv = v7.transpose(6); // tritone = 6 semitones
        const char* n = subv.name();
        uint8_t i = 0;
        while (n[i] && i < maxLen - 1) { chordName[i] = n[i]; i++; }
        chordName[i] = '\0';
        return true;
    }

    // Diminished passing: #Idim, bIIIdim, IV#dim, II#dim
    if (strstr(branch, "dim")) {
        // Find the root by parsing the roman numeral with accidental
        uint8_t rootSt = field_.tonic().semitone();
        int8_t offset = 0;
        const char* p = branch;

        // Parse accidentals
        while (*p == '#' || *p == 'b') {
            if (*p == '#') offset++;
            else offset--;
            p++;
        }

        // Parse roman numeral
        if (strncmp(p, "VII", 3) == 0) offset += 11;
        else if (strncmp(p, "VI", 2) == 0) offset += 9;
        else if (strncmp(p, "V", 1) == 0 && p[1] != 'I') offset += 7;
        else if (strncmp(p, "IV", 2) == 0) offset += 5;
        else if (strncmp(p, "III", 3) == 0) offset += 4;
        else if (strncmp(p, "II", 2) == 0) offset += 2;
        else if (*p == 'I' && p[1] != 'I' && p[1] != 'V') offset += 0;

        uint8_t noteSt = (rootSt + (uint8_t)((offset + 12) % 12)) % 12;
        char noteName[4];
        data::readChromaticName(noteSt, noteName, sizeof(noteName));

        uint8_t i = 0;
        while (noteName[i] && i < maxLen - 4) { chordName[i] = noteName[i]; i++; }
        chordName[i++] = 'd'; chordName[i++] = 'i'; chordName[i++] = 'm';
        chordName[i] = '\0';
        return true;
    }

    // Borrowed chords: IVm, bVI, bVII, bIII, Vm
    // Parse roman numeral with optional accidental and quality suffix
    {
        const char* p = branch;
        int8_t accidental = 0;
        while (*p == 'b') { accidental--; p++; }
        while (*p == '#') { accidental++; p++; }

        uint8_t deg = 0;
        if (strncmp(p, "VII", 3) == 0) { deg = 7; p += 3; }
        else if (strncmp(p, "VI", 2) == 0) { deg = 6; p += 2; }
        else if (*p == 'V' && p[1] != 'I') { deg = 5; p += 1; }
        else if (strncmp(p, "IV", 2) == 0) { deg = 4; p += 2; }
        else if (strncmp(p, "III", 3) == 0) { deg = 3; p += 3; }
        else if (strncmp(p, "II", 2) == 0) { deg = 2; p += 2; }
        else if (*p == 'I') { deg = 1; p += 1; }

        if (deg > 0) {
            // Get the note at this degree in the scale
            GingoNote degNote = field_.scale().degree(deg);
            GingoNote root = degNote.transpose(accidental);

            const char* rn = root.name();
            uint8_t i = 0;
            while (rn[i] && i < maxLen - 8) { chordName[i] = rn[i]; i++; }

            // Append quality suffix
            while (*p && i < maxLen - 1) { chordName[i++] = *p++; }
            // If no suffix left, add "M" for major
            if (i > 0 && chordName[i - 1] >= 'A' && chordName[i - 1] <= 'G') {
                chordName[i++] = 'M';
            }
            if (i > 1 && (chordName[i - 1] == '#' || chordName[i - 1] == 'b')) {
                chordName[i++] = 'M';
            }
            chordName[i] = '\0';
            return true;
        }
    }

    return false;
}

const char* GingoTree::traditionName(char* buf, uint8_t maxLen) const {
    const char* ptr = (const char*)pgm_read_ptr(&data::PROG_TRADITION_NAMES[traditionId_]);
    data::readPgmStr(buf, ptr, maxLen);
    return buf;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_TREE
