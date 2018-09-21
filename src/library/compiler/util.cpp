/*
Copyright (c) 2018 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include "kernel/type_checker.h"
#include "kernel/instantiate.h"
#include "library/attribute_manager.h"
#include "library/aux_recursors.h"
#include "library/replace_visitor.h"
#include "library/constants.h"

namespace lean {
bool has_inline_attribute(environment const & env, name const & n) {
    if (has_attribute(env, "inline", n))
        return true;
    if (is_internal_name(n) && !n.is_atomic()) {
        /* Auxiliary declarations such as `f._main` are considered to be marked as `@[inline]`
           if `f` is marked. */
        return has_inline_attribute(env, n.get_prefix());
    }
    return false;
}

bool has_noinline_attribute(environment const & /* env */, name const & /* n */) {
    return false;
}

bool is_lcnf_atom(expr const & e) {
    switch (e.kind()) {
    case expr_kind::FVar: case expr_kind::Const: case expr_kind::Lit:
        return true;
    default:
        return false;
    }
}

class elim_trivial_let_decls_fn : public replace_visitor {
    virtual expr visit_let(expr const & e) override {
        if (is_lcnf_atom(let_value(e))) {
            return visit(instantiate(let_body(e), let_value(e)));
        } else {
            return replace_visitor::visit_let(e);
        }
    }
};

expr elim_trivial_let_decls(expr const & e) {
    return elim_trivial_let_decls_fn()(e);
}

expr cheap_beta_reduce(expr const & e) {
    if (!is_app(e)) return e;
    expr fn = get_app_fn(e);
    if (!is_lambda(fn)) return e;
    buffer<expr> args;
    get_app_args(e, args);
    unsigned i = 0;
    while (is_lambda(fn) && i < args.size()) {
        i++;
        fn = binding_body(fn);
    }
    if (!has_loose_bvars(fn)) {
        return mk_app(fn, args.size() - i, args.data() + i);
    } else if (is_bvar(fn)) {
        lean_assert(bvar_idx(fn) < i);
        return mk_app(args[i - bvar_idx(fn).get_small_value() - 1], args.size() - i, args.data() + i);
    } else {
        return e;
    }
}

bool is_cases_on_recursor(environment const & env, name const & n) {
    return ::lean::is_aux_recursor(env, n) && n.get_string() == "cases_on";
}

expr mk_lc_unreachable(type_checker::state & s, local_ctx const & lctx, expr const & type) {
    type_checker tc(s, lctx);
    level lvl = sort_level(tc.ensure_type(type));
    return mk_app(mk_constant(get_lc_unreachable_name(), {lvl}), type);
}
}
