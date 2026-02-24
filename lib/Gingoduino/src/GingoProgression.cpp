// Gingoduino — Music Theory Library for Embedded Systems
// Implementation of GingoProgression.
//
// SPDX-License-Identifier: MIT

#include "GingoProgression.h"

#if GINGODUINO_HAS_PROGRESSION

#include "gingoduino_progmem.h"

namespace gingoduino {

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

// Check if needle[] is a contiguous subsequence of schema branches
static bool isContiguousSubseq(const uint8_t* needle, uint8_t needleLen,
                                const uint8_t* haystack, uint8_t haystackLen) {
    if (needleLen > haystackLen) return false;
    for (uint8_t i = 0; i + needleLen <= haystackLen; i++) {
        bool match = true;
        for (uint8_t j = 0; j < needleLen; j++) {
            if (needle[j] != haystack[i + j]) { match = false; break; }
        }
        if (match) return true;
    }
    return false;
}

// Check if prefix[] is a prefix of schema branches
static bool isPrefixOf(const uint8_t* prefix, uint8_t prefixLen,
                        const uint8_t* seq, uint8_t seqLen) {
    if (prefixLen > seqLen) return false;
    for (uint8_t i = 0; i < prefixLen; i++) {
        if (prefix[i] != seq[i]) return false;
    }
    return true;
}

// Simple insertion sort for ProgressionMatch by scoreNum desc
static void sortMatches(ProgressionMatch* arr, uint8_t n) {
    for (uint8_t i = 1; i < n; i++) {
        ProgressionMatch key = arr[i];
        int8_t j = (int8_t)(i - 1);
        while (j >= 0 && arr[j].scoreNum < key.scoreNum) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Simple insertion sort for ProgressionRoute by confidenceNum desc
static void sortRoutes(ProgressionRoute* arr, uint8_t n) {
    for (uint8_t i = 1; i < n; i++) {
        ProgressionRoute key = arr[i];
        int8_t j = (int8_t)(i - 1);
        while (j >= 0 && arr[j].confidenceNum < key.confidenceNum) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// Read a schema from PROGMEM
static void readSchema(const data::ProgSchema* pgmSchema,
                       char* name, uint8_t nameMax,
                       uint8_t* branches, uint8_t* count, uint8_t* ctx) {
    data::readPgmStr(name, pgmSchema->name, nameMax);
    *count = pgm_read_byte(&pgmSchema->count);
    *ctx = pgm_read_byte(&pgmSchema->ctx);
    for (uint8_t i = 0; i < *count; i++) {
        branches[i] = pgm_read_byte(&pgmSchema->branches[i]);
    }
}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

GingoProgression::GingoProgression(const char* tonic, ScaleType type)
    : tonic_(tonic), scaleType_(type)
{}

GingoTree GingoProgression::tree(uint8_t traditionId) const {
    return GingoTree(tonic_.name(), scaleType_, traditionId);
}

// ---------------------------------------------------------------------------
// identify — find the single best match
// ---------------------------------------------------------------------------

bool GingoProgression::identify(const char* const* branches, uint8_t count,
                                ProgressionMatch* result) const {
    if (count < 2) return false;

    // Convert input to branch IDs
    uint8_t inputIds[16];
    uint8_t inputLen = (count < 16) ? count : 16;
    for (uint8_t i = 0; i < inputLen; i++) {
        inputIds[i] = GingoTree::findBranch(branches[i]);
    }

    uint8_t bestScore = 0;
    uint8_t bestSchemaScore = 0;
    bool found = false;

    for (uint8_t trad = 0; trad < PROG_TRADITION_COUNT; trad++) {
        GingoTree t = tree(trad);

        // Score by valid transitions
        uint8_t totalTrans = inputLen - 1;
        uint8_t validTrans = t.countValidTransitions(branches, inputLen);
        uint8_t transScore = (uint8_t)((uint16_t)validTrans * 100 / totalTrans);

        // Score by schema match
        const data::ProgSchemaTable* st =
            (const data::ProgSchemaTable*)&data::PROG_SCHEMA_TABLES[trad];
        const data::ProgSchema* schemas =
            (const data::ProgSchema*)pgm_read_ptr(&st->schemas);
        uint8_t schemaCount = pgm_read_byte(&st->count);

        char bestSchemaName[24] = "";
        uint8_t schemaScoreVal = 0;

        for (uint8_t s = 0; s < schemaCount; s++) {
            char sName[24];
            uint8_t sBranches[8], sCount, sCtx;
            readSchema(&schemas[s], sName, sizeof(sName), sBranches, &sCount, &sCtx);

            // Context must match
            if (sCtx != t.context()) continue;

            // Exact match
            if (sCount == inputLen) {
                bool exact = true;
                for (uint8_t i = 0; i < sCount; i++) {
                    if (inputIds[i] != sBranches[i]) { exact = false; break; }
                }
                if (exact) {
                    schemaScoreVal = 100;
                    memcpy(bestSchemaName, sName, sizeof(bestSchemaName));
                    break;
                }
            }

            // Contiguous subsequence
            if (isContiguousSubseq(inputIds, inputLen, sBranches, sCount)) {
                uint8_t ss = (uint8_t)((uint16_t)inputLen * 100 / sCount);
                if (ss > schemaScoreVal) {
                    schemaScoreVal = ss;
                    memcpy(bestSchemaName, sName, sizeof(bestSchemaName));
                }
            }
        }

        uint8_t finalScore = (transScore > schemaScoreVal) ? transScore : schemaScoreVal;

        if (finalScore > bestScore ||
            (finalScore == bestScore && schemaScoreVal > bestSchemaScore)) {
            bestScore = finalScore;
            bestSchemaScore = schemaScoreVal;
            result->traditionId = trad;
            memcpy(result->schema, bestSchemaName, sizeof(result->schema));
            result->matched = validTrans;
            result->total = totalTrans;
            result->scoreNum = finalScore;
            found = true;
        }
    }

    return found && bestScore > 0;
}

// ---------------------------------------------------------------------------
// deduce — find all probable matches, ranked
// ---------------------------------------------------------------------------

uint8_t GingoProgression::deduce(const char* const* branches, uint8_t count,
                                  ProgressionMatch* output, uint8_t maxResults) const {
    if (count < 2 || maxResults == 0) return 0;

    // Convert input to branch IDs
    uint8_t inputIds[16];
    uint8_t inputLen = (count < 16) ? count : 16;
    for (uint8_t i = 0; i < inputLen; i++) {
        inputIds[i] = GingoTree::findBranch(branches[i]);
    }

    // Temporary buffer for all candidates
    ProgressionMatch candidates[24]; // max: 2 traditions * ~10 schemas + 2 no-schema
    uint8_t candCount = 0;

    for (uint8_t trad = 0; trad < PROG_TRADITION_COUNT; trad++) {
        GingoTree t = tree(trad);

        uint8_t totalTrans = inputLen - 1;
        uint8_t validTrans = t.countValidTransitions(branches, inputLen);
        uint8_t transScore = (uint8_t)((uint16_t)validTrans * 100 / totalTrans);

        const data::ProgSchemaTable* st =
            (const data::ProgSchemaTable*)&data::PROG_SCHEMA_TABLES[trad];
        const data::ProgSchema* schemas =
            (const data::ProgSchema*)pgm_read_ptr(&st->schemas);
        uint8_t schemaCount = pgm_read_byte(&st->count);

        bool hasSchemaMatch = false;

        for (uint8_t s = 0; s < schemaCount && candCount < 24; s++) {
            char sName[24];
            uint8_t sBranches[8], sCount, sCtx;
            readSchema(&schemas[s], sName, sizeof(sName), sBranches, &sCount, &sCtx);

            if (sCtx != t.context()) continue;

            uint8_t schemaScore = 0;

            // Exact match
            if (sCount == inputLen) {
                bool exact = true;
                for (uint8_t i = 0; i < sCount; i++) {
                    if (inputIds[i] != sBranches[i]) { exact = false; break; }
                }
                if (exact) schemaScore = 100;
            }

            // Prefix match
            if (schemaScore == 0 && isPrefixOf(inputIds, inputLen, sBranches, sCount)) {
                schemaScore = (uint8_t)((uint16_t)inputLen * 100 / sCount);
            }

            // Contiguous subsequence (with 0.9 factor)
            if (schemaScore == 0 && isContiguousSubseq(inputIds, inputLen, sBranches, sCount)) {
                schemaScore = (uint8_t)((uint16_t)inputLen * 90 / sCount);
            }

            if (schemaScore > 0) {
                hasSchemaMatch = true;
                ProgressionMatch& m = candidates[candCount++];
                m.traditionId = trad;
                memcpy(m.schema, sName, sizeof(m.schema));
                m.matched = validTrans;
                m.total = totalTrans;
                m.scoreNum = (transScore > schemaScore) ? transScore : schemaScore;
            }
        }

        // If no schema matched but transitions are valid, add a no-schema entry
        if (!hasSchemaMatch && transScore > 0 && candCount < 24) {
            ProgressionMatch& m = candidates[candCount++];
            m.traditionId = trad;
            m.schema[0] = '\0';
            m.matched = validTrans;
            m.total = totalTrans;
            m.scoreNum = transScore;
        }
    }

    // Sort by score desc
    sortMatches(candidates, candCount);

    // Copy top results
    uint8_t written = (candCount < maxResults) ? candCount : maxResults;
    for (uint8_t i = 0; i < written; i++) {
        output[i] = candidates[i];
    }
    return written;
}

// ---------------------------------------------------------------------------
// predict — suggest next branches
// ---------------------------------------------------------------------------

uint8_t GingoProgression::predict(const char* const* branches, uint8_t count,
                                   ProgressionRoute* output, uint8_t maxResults) const {
    if (count == 0 || maxResults == 0) return 0;

    const char* last = branches[count - 1];

    // Build extended sequence IDs (input + candidate next)
    uint8_t inputIds[16];
    uint8_t inputLen = (count < 15) ? count : 15;
    for (uint8_t i = 0; i < inputLen; i++) {
        inputIds[i] = GingoTree::findBranch(branches[i]);
    }

    ProgressionRoute candidates[32];
    uint8_t candCount = 0;

    for (uint8_t trad = 0; trad < PROG_TRADITION_COUNT; trad++) {
        GingoTree t = tree(trad);

        // Get neighbors of the last branch
        const char* neighPtrs[16];
        uint8_t nNeigh = t.neighbors(last, neighPtrs, 16);

        for (uint8_t n = 0; n < nNeigh && candCount < 32; n++) {
            // Read the neighbor name
            char nextName[24];
            data::readPgmStr(nextName, neighPtrs[n], sizeof(nextName));

            uint8_t nextId = GingoTree::findBranch(nextName);

            // Build candidate sequence: input + next
            uint8_t candIds[16];
            for (uint8_t i = 0; i < inputLen; i++) candIds[i] = inputIds[i];
            candIds[inputLen] = nextId;
            uint8_t candLen = inputLen + 1;

            // Score against schemas
            const data::ProgSchemaTable* st =
                (const data::ProgSchemaTable*)&data::PROG_SCHEMA_TABLES[trad];
            const data::ProgSchema* schemas =
                (const data::ProgSchema*)pgm_read_ptr(&st->schemas);
            uint8_t schemaCount = pgm_read_byte(&st->count);

            uint8_t confidence = 30; // baseline
            char matchedSchema[24] = "";

            for (uint8_t s = 0; s < schemaCount; s++) {
                char sName[24];
                uint8_t sBranches[8], sCount, sCtx;
                readSchema(&schemas[s], sName, sizeof(sName), sBranches, &sCount, &sCtx);

                if (sCtx != t.context()) continue;

                // Prefix match
                if (isPrefixOf(candIds, candLen, sBranches, sCount)) {
                    uint8_t c = (uint8_t)((uint16_t)candLen * 100 / sCount);
                    if (c > confidence) {
                        confidence = c;
                        memcpy(matchedSchema, sName, sizeof(matchedSchema));
                    }
                }

                // Contiguous subsequence (0.8 factor)
                if (isContiguousSubseq(candIds, candLen, sBranches, sCount)) {
                    uint8_t c = (uint8_t)((uint16_t)candLen * 80 / sCount);
                    if (c > confidence) {
                        confidence = c;
                        memcpy(matchedSchema, sName, sizeof(matchedSchema));
                    }
                }
            }

            // Avoid duplicates: check if this next is already in candidates
            bool dup = false;
            for (uint8_t d = 0; d < candCount; d++) {
                if (strcmp(candidates[d].next, nextName) == 0 &&
                    candidates[d].traditionId == trad) {
                    // Keep higher confidence
                    if (confidence > candidates[d].confidenceNum) {
                        candidates[d].confidenceNum = confidence;
                        memcpy(candidates[d].schema, matchedSchema, sizeof(candidates[d].schema));
                    }
                    dup = true;
                    break;
                }
            }

            if (!dup) {
                ProgressionRoute& r = candidates[candCount++];
                memcpy(r.next, nextName, sizeof(r.next));
                r.traditionId = trad;
                memcpy(r.schema, matchedSchema, sizeof(r.schema));
                r.confidenceNum = confidence;
            }
        }
    }

    // Sort by confidence desc
    sortRoutes(candidates, candCount);

    // Copy top results
    uint8_t written = (candCount < maxResults) ? candCount : maxResults;
    for (uint8_t i = 0; i < written; i++) {
        output[i] = candidates[i];
    }
    return written;
}

} // namespace gingoduino

#endif // GINGODUINO_HAS_PROGRESSION
