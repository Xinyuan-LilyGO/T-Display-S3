// Gingoduino — Music Theory Library for Embedded Systems
// GingoTree: directed harmonic graph for a given tradition and scale context.
//
// SPDX-License-Identifier: MIT

#ifndef GINGO_TREE_H
#define GINGO_TREE_H

#include "gingoduino_config.h"

#if GINGODUINO_HAS_TREE

#include "gingoduino_types.h"
#include "GingoNote.h"
#include "GingoField.h"

namespace gingoduino {

/// Represents a directed harmonic graph (tree) for a specific tradition
/// and scale context (major or minor).
///
/// The graph encodes valid harmonic transitions between chord functions
/// (branches like "I", "V7", "IIm", etc.).
///
/// Examples:
///   GingoTree t("C", SCALE_MAJOR, 0);  // harmonic_tree, C major
///   t.isValid("I", "V7");              // true
///   t.isValid("I", "IVm");             // false
class GingoTree {
public:
    /// Construct a tree for a tonic, scale type, and tradition.
    /// traditionId: 0 = harmonic_tree, 1 = jazz.
    GingoTree(const char* tonic, ScaleType type, uint8_t traditionId = 0);

    /// Check if a single transition origin→target is valid.
    bool isValid(const char* origin, const char* target) const;

    /// Check if an entire sequence of branches forms valid transitions.
    bool isValidSequence(const char* const* sequence, uint8_t count) const;

    /// Count valid transitions in a sequence. Returns 0..count-1.
    uint8_t countValidTransitions(const char* const* sequence, uint8_t count) const;

    /// List all neighbors (valid targets) of a branch.
    /// Writes branch name pointers (PROGMEM) to output.
    /// Returns number written.
    uint8_t neighbors(const char* branch, const char** output, uint8_t maxNeighbors) const;

    /// Resolve a branch name to a concrete chord name.
    /// E.g., "V7" in C major → "G7", "IIm" in C major → "Dm".
    /// Returns true if resolved successfully.
    bool resolve(const char* branch, char* chordName, uint8_t maxLen) const;

    /// Get the tradition ID.
    uint8_t traditionId() const { return traditionId_; }

    /// Get the tradition name (from PROGMEM).
    const char* traditionName(char* buf, uint8_t maxLen) const;

    /// Get the scale context: 0=major, 1=minor.
    uint8_t context() const { return ctx_; }

    /// Find the branch ID for a branch name string. Returns 0xFF if not found.
    static uint8_t findBranch(const char* name);

private:
    GingoField field_;
    uint8_t    traditionId_;
    uint8_t    ctx_;  // 0=major, 1=minor

    /// Check if edge (origin_id, target_id) exists in the edge table.
    bool hasEdge(uint8_t originId, uint8_t targetId) const;
};

} // namespace gingoduino

#endif // GINGODUINO_HAS_TREE
#endif // GINGO_TREE_H
