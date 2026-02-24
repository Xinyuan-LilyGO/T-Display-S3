// Gingoduino — Music Theory Library for Embedded Systems
// GingoProgression: harmonic progression analysis — identify, deduce, predict.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_PROGRESSION_H
#define GINGO_PROGRESSION_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_PROGRESSION

#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoTree.h"

namespace gingoduino {

/// Result of identify() or deduce(): a matching tradition/schema.
struct ProgressionMatch {
    uint8_t traditionId;    // 0=harmonic_tree, 1=jazz
    char    schema[24];     // schema name (or "" if no schema match)
    uint8_t matched;        // valid transitions in the sequence
    uint8_t total;          // total transitions (count - 1)
    uint8_t scoreNum;       // score * 100 (0-100)
};

/// Result of predict(): a suggested next branch.
struct ProgressionRoute {
    char    next[24];        // suggested next branch name
    uint8_t traditionId;     // tradition this prediction comes from
    char    schema[24];      // associated schema (or "")
    uint8_t confidenceNum;   // confidence * 100 (0-100)
};

/// Analyzes harmonic progressions against known traditions (harmonic_tree, jazz).
///
/// Examples:
///   GingoProgression p("C", SCALE_MAJOR);
///
///   // Identify best match
///   const char* seq[] = {"IIm", "V7", "I"};
///   ProgressionMatch m;
///   p.identify(seq, 3, &m);  // jazz, "ii-V-I", score=100
///
///   // Predict next
///   const char* partial[] = {"IIm", "V7"};
///   ProgressionRoute routes[8];
///   uint8_t n = p.predict(partial, 2, routes, 8);
class GingoProgression {
public:
    GingoProgression(const char* tonic, ScaleType type);

    /// Identify the single best tradition/schema match for a sequence.
    /// Returns true if a match was found.
    bool identify(const char* const* branches, uint8_t count,
                  ProgressionMatch* result) const;

    /// Deduce all probable tradition/schema matches, ranked by score.
    /// Returns number of results written.
    uint8_t deduce(const char* const* branches, uint8_t count,
                   ProgressionMatch* output, uint8_t maxResults) const;

    /// Predict possible next branches from the current sequence.
    /// Returns number of predictions written.
    uint8_t predict(const char* const* branches, uint8_t count,
                    ProgressionRoute* output, uint8_t maxResults) const;

    /// Get a tree for a specific tradition.
    GingoTree tree(uint8_t traditionId) const;

private:
    GingoNote tonic_;
    ScaleType scaleType_;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_PROGRESSION
#endif // GINGO_PROGRESSION_H
