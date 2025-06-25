#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <optional>

// Include the parser header which defines all the Node structs and includes dependencies
#include "parser.hpp"

// Forward declarations for all Node types are necessary because of mutual recursion
// in the printing functions and the use of pointers.
struct NodeTermIntLit;
struct NodeTermIdent;
struct NodeExpr;
struct NodeTermParen;
struct NodeBinExprAdd;
struct NodeBinExprMulti;
struct NodeBinExprSub;
struct NodeBinExprDiv;
struct NodeBinExpr;
struct NodeTerm;
struct NodeStmtExit;
struct NodeStmtLet;
struct NodeStmt;
struct NodeScope;
struct NodeIfPred;
struct NodeIfPredElif;
struct NodeIfPredElse;
struct NodeStmtIf;
struct NodeStmtAssign;
struct NodeProg;
void print_stmt(std::ostream& os, const NodeStmt* node_ptr, int level, bool is_last);

// Helper function for indentation with tree-like structure
inline std::ostream& indent(std::ostream& os, int level, bool is_last) {
    for (int i = 0; i < level - 1; ++i) {
        os << "│   ";
    }
    if (level > 0) {
        os << (is_last ? "└── " : "├── ");
    }
    return os;
}

// --- Overloads for specific Node types ---

inline std::ostream& operator<<(std::ostream& os, const NodeTermIntLit& node) {
    os << "IntLit(" << node.int_lit.value.value_or("?") << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeTermIdent& node) {
    os << "Ident(" << node.ident.value.value_or("?") << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeExpr* node_ptr); // Forward declare for NodeTermParen

inline std::ostream& operator<<(std::ostream& os, const NodeTermParen& node) {
    os << "ParenExpr";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeBinExprAdd& node) {
    os << "BinaryOp(+)";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeBinExprMulti& node) {
    os << "BinaryOp(*)";
     return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeBinExprSub& node) {
    os << "BinaryOp(-)";
     return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeBinExprDiv& node) {
    os << "BinaryOp(/)";
     return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeStmt* node_ptr); // Forward declare for NodeScope
inline std::ostream& operator<<(std::ostream& os, const NodeScope* node_ptr); // Forward declare for NodeIfPred variants

inline std::ostream& operator<<(std::ostream& os, const NodeStmtExit& node) {
    os << "Exit";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeStmtLet& node) {
    os << "Let(" << node.ident.value.value_or("?") << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeStmtAssign& node) {
    os << "Assign(" << node.ident.value.value_or("?") << ")";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeIfPred* node_ptr); // Forward declare for NodeStmtIf/NodeIfPredElif

inline std::ostream& operator<<(std::ostream& os, const NodeIfPredElif& node) {
    os << "Elif";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeIfPredElse& node) {
    os << "Else";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeStmtIf& node) {
    os << "If";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const NodeScope& node) {
    os << "Scope"; // Or whatever representation you want for NodeScope itself
    return os;
}


// --- Overloads for pointer types (used within the AST) ---

void print_expr(std::ostream& os, const NodeExpr* node_ptr, int level) {
    if (!node_ptr) return;
    std::visit([&](auto* term_or_bin_expr_ptr) {
        if (!term_or_bin_expr_ptr) return;
        std::visit([&](auto* concrete_ptr) {
            indent(os, level, false) << *concrete_ptr << "\n";
            if constexpr (std::is_same_v<decltype(concrete_ptr), NodeTermParen*>) {
                print_expr(os, concrete_ptr->expr, level + 1);
            } else if constexpr (std::is_same_v<decltype(concrete_ptr), NodeBinExprAdd*>
                               || std::is_same_v<decltype(concrete_ptr), NodeBinExprMulti*>
                               || std::is_same_v<decltype(concrete_ptr), NodeBinExprSub*>
                               || std::is_same_v<decltype(concrete_ptr), NodeBinExprDiv*>) {
                indent(os, level + 1, false) << "LHS:\n";
                print_expr(os, concrete_ptr->lhs, level + 2);
                indent(os, level + 1, true) << "RHS:\n";
                print_expr(os, concrete_ptr->rhs, level + 2);
            }
        }, term_or_bin_expr_ptr->var);
    }, node_ptr->var);
}

// Main overload for NodeExpr*
inline std::ostream& operator<<(std::ostream& os, const NodeExpr* node_ptr) {
    print_expr(os, node_ptr, 1);
    return os;
}

void print_scope(std::ostream& os, const NodeScope* node_ptr, int level) {
    if (!node_ptr) return;
    indent(os, level, true) << "Scope\n";
    for (size_t i = 0; i < node_ptr->stmts.size(); ++i) {
        print_stmt(os, node_ptr->stmts[i], level + 1, i == node_ptr->stmts.size() - 1);
    }
}

void print_if_pred(std::ostream& os, const NodeIfPred* node_ptr, int level, bool is_last) {
    if (!node_ptr) return;
    std::visit([&](auto* concrete_pred_ptr) {
        indent(os, level, is_last) << *concrete_pred_ptr << "\n";
        if constexpr (std::is_same_v<decltype(concrete_pred_ptr), NodeIfPredElif*>) {
            indent(os, level + 1, false) << "Cond:\n";
            print_expr(os, concrete_pred_ptr->expr, level + 2);
            print_scope(os, concrete_pred_ptr->scope, level + 1);
            if (concrete_pred_ptr->pred.has_value()) {
                print_if_pred(os, concrete_pred_ptr->pred.value(), level, true);
            }
        } else if constexpr (std::is_same_v<decltype(concrete_pred_ptr), NodeStmtIf*>) {
            // This case should ideally not be reached here as NodeStmtIf is handled in print_stmt
            indent(os, level + 1, false) << "Cond:\n";
            print_expr(os, concrete_pred_ptr->expr, level + 2);
            print_scope(os, concrete_pred_ptr->scope, level + 1);
            if (concrete_pred_ptr->pred.has_value()) {
                print_if_pred(os, concrete_pred_ptr->pred.value(), level, true);
            }
        } else if constexpr (std::is_same_v<decltype(concrete_pred_ptr), NodeIfPredElse*>) {
            print_scope(os, concrete_pred_ptr->scope, level + 1);
        }
    }, node_ptr->var);
}

void print_stmt(std::ostream& os, const NodeStmt* node_ptr, int level, bool is_last) {
    if (!node_ptr) return;
    std::visit([&](auto* concrete_stmt_ptr) {
        indent(os, level, is_last) << *concrete_stmt_ptr;
        if constexpr (std::is_same_v<decltype(concrete_stmt_ptr), NodeStmtExit*>) {
            os << "\n";
            print_expr(os, concrete_stmt_ptr->expr, level + 1);
        } else if constexpr (std::is_same_v<decltype(concrete_stmt_ptr), NodeStmtLet*>) {
            os << "\n";
            print_expr(os, concrete_stmt_ptr->expr, level + 1);
        } else if constexpr (std::is_same_v<decltype(concrete_stmt_ptr), NodeStmtAssign*>) {
            os << "\n";
            print_expr(os, concrete_stmt_ptr->expr, level + 1);
        } else if constexpr (std::is_same_v<decltype(concrete_stmt_ptr), NodeStmtIf*>) {
            os << "\n";
            indent(os, level + 1, false) << "Cond:\n";
            print_expr(os, concrete_stmt_ptr->expr, level + 2);
            print_scope(os, concrete_stmt_ptr->scope, level + 1);
            if (concrete_stmt_ptr->pred.has_value()) {
                print_if_pred(os, concrete_stmt_ptr->pred.value(), level + 1, true);
            }
        } else if constexpr (std::is_same_v<decltype(concrete_stmt_ptr), NodeScope*>) {
            os << "\n";
            print_scope(os, concrete_stmt_ptr, level + 1);
        }
    }, node_ptr->var);
}

// Main overload for NodeStmt*
inline std::ostream& operator<<(std::ostream& os, const NodeStmt* node_ptr) {
    print_stmt(os, node_ptr, 1, true); // The 'true' here is a placeholder, context matters in print_scope/print_prog
    return os;
}

// --- Overload for the top-level NodeProg ---

inline std::ostream& operator<<(std::ostream& os, const NodeProg& prog) {
    os << "Program\n";
    for (size_t i = 0; i < prog.stmts.size(); ++i) {
        print_stmt(os, prog.stmts[i], 1, i == prog.stmts.size() - 1);
    }
    return os;
}