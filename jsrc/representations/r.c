/* Copyright 1990-2006, Jsoftware Inc.  All rights reserved.               */
/* Licensed use only. Any other use is in violation of copyright.          */
/*                                                                         */
/* Representations: Atomic, Boxed, and 5!:0                                */

#include "j.h"
#include "words/w.h"

static A jtdrr(J jt, A w);
EVERYFS(drrself, jtdrr, 0, 0, VFLAGNONE)

static A
jtdrr(J jt, A w) {
    PROLOG(0055);
    A df, dg, hs, *x, z;
    B b, ex, xop;
    C c, id;
    I fl, *hv, m;
    V *v;
    if (!w) return 0;
    // If the input is a name, it must be from ".@'name' which turned into ".@(name+noun)  - or in debug, but that's
    // discarded
    if (AT(w) & NAME) { RZ(w = jtsfn(jt, 0, w)); }
    // If noun, return the value of the noun.
    if (AT(w) & NOUN) return w;  // no quotes needed
    // Non-nouns and NMDOT names carry on
    v      = FAV(w);
    id     = v->id;
    fl     = v->flag;
    I fndx = (id == CBDOT) && !v->fgh[0];
    A fs   = v->fgh[fndx];
    A gs =
      v->fgh[fndx ^
             1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be more general
    hs = v->fgh[2];
    if (id == CBOX) gs = 0;  // ignore gs field in BOX, there to simulate BOXATOP
    if (fl & VXOPCALL) return jtdrr(jt, hs);
    xop = 1 && VXOP & fl;
    ex  = id == CCOLON && hs && !xop;
    b   = BETWEENC(id, CHOOK, CADVF);
    c   = id == CFORK;
    b &= 1 ^ c;  // HOOK ADVF, and FORK
    m = !!fs + (gs || ex);
    if (!m) return jtspella(jt, w);
    if (evoke(w)) return jtdrr(jt, jtsfne(jt, w));  // turn nameref into string or verb; then take rep
    if (fs) RZ(df = fl & VGERL ? jtevery(jt, jtfxeach(jt, fs, (A)&jtfxself[0]), (A)&drrself) : jtdrr(jt, fs));
    if (gs) RZ(dg = fl & VGERR ? jtevery(jt, jtfxeach(jt, gs, (A)&jtfxself[0]), (A)&drrself) : jtdrr(jt, gs));
    if (ex) RZ(dg = jtunparsem(jt, jfalse, w));
    m += !b && !xop || hs && xop;
    GATV0(z, BOX, m, 1);
    x = AAV(z);
    RZ(x[0] = jtincorp(jt, df));
    RZ(x[1] = jtincorp(jt,
                       b || c || xop ? dg
                       : fl & VDDOP  ? (hv = AV(hs), link(jtsc(jt, hv[0]), link(jtspellout(jt, id), jtsc(jt, hv[1]))))
                                     : jtspellout(jt, id)));
    if (2 < m) RZ(x[2] = jtincorp(jt, c || xop ? jtdrr(jt, hs) : dg));
    EPILOG(z);
}

A
jtdrep(J jt, A w) {
    A z = jtdrr(jt, w);
    return z && AT(z) & BOX ? z : jtravel(jt, jtbox(jt, z));
}

A
jtaro(J jt, A w) {
    A fs, gs, hs, s, *u, *x, y, z;
    B ex, xop;
    C id;
    I *hv, m;
    V *v;
    if (!w) return 0;
    if (FUNC & AT(w)) {
        v      = FAV(w);
        id     = v->id;
        I fndx = (id == CBDOT) && !v->fgh[0];
        fs     = v->fgh[fndx];
        gs = v->fgh[fndx ^ 1];  // In verb for m b., if f is empty look to g for the left arg.  It would be nice to be
                                // more general
        hs = v->fgh[2];
        if (id == CBOX) gs = 0;  // ignore gs field in BOX, there to simulate BOXATOP
        if (VXOPCALL & v->flag) return jtaro(jt, hs);
        xop = 1 && VXOP & v->flag;
        ex  = hs && id == CCOLON && !xop;
        m   = id == CFORK ? 3 : !!fs + (ex || xop && hs || !xop && gs);
        if (!m) return jtspella(jt, w);
        if (evoke(w)) {
            RZ(w = jtsfne(jt, w));
            if (FUNC & AT(w)) w = jtaro(jt, w);
            return w;
        }  // keep nameref as a string, UNLESS it is NMDOT, in which case use the (f.'d) verb value
    }
    GAT0(z, BOX, 2, 1);
    x = AAV(z);
    if (NOUN & AT(w)) {
        RZ(x[0] = jtincorp(jt, jtravel(jt, jtscc(jt, CNOUN))));
        if (AT(w) & NAME) RZ(w = jtsfn(jt, 0, w));
        RZ(x[1] = jtincorp(jt, w));
        return z;
    }  // if name, must be ".@'name', format name as string
    GATV0(y, BOX, m, 1);
    u = AAV(y);
    if (0 < m) RZ(u[0] = jtincorp(jt, jtaro(jt, fs)));
    if (1 < m) RZ(u[1] = jtincorp(jt, jtaro(jt, ex ? jtunparsem(jt, jfalse, w) : xop ? hs : gs)));
    if (2 < m) RZ(u[2] = jtincorp(jt, jtaro(jt, hs)));
    s = xop               ? jtaro(jt, gs)
        : VDDOP & v->flag ? (hv = AV(hs), jtaro(jt, jtforeign(jt, jtsc(jt, hv[0]), jtsc(jt, hv[1]))))
                          : jtspellout(jt, id);
    RZ(x[0] = jtincorp(jt, s));
    RZ(x[1] = jtincorp(jt, y));
    return z;
}

A
jtarep(J jt, A w) {
    return jtbox(jt, jtaro(jt, w));
}

// Create A for a string - name~, a primitive, or the boxed string
static A
jtfxchar(J jt, A w, A self) {
    A y;
    C c, d, id, *s;
    I m, n;
    n = AN(w);
    ASSERT(1 >= AR(w), EVRANK);  // string must be an atom or list
    ASSERT(n != 0, EVLENGTH);
    s = CAV(w);
    c = s[n - 1];
    DO(n, d = s[i]; ASSERT((C)(d - 32) < (C)(127 - 32), EVSPELL););  // must be all ASCII
    if (((ctype[(UC)s[0]] & ~CA) == 0) && c != CESC1 && c != CESC2)
        return jtswap(jt, w);             // If name and not control word, treat as name~, create nameref
    ASSERT(id = spellin(n, s), EVSPELL);  // not name, must be control word or primitive.  Also classify string
    if (id != CFCONS)
        y = ds(id);
    else {
        m = s[n - 2] - '0';
        y = FCONS(CSIGN != s[0] ? jtscib(jt, m) : 2 == n ? ainf : jtscib(jt, -m));
    }                                            // define 0:, if it's that, using boolean for 0/1
    ASSERT(y && RHS & AT(y), EVDOMAIN);          // make sure it's a noun/verb/adv/conj
    if (!self || AT(y) & NOUN + VERB) return y;  // return any NV, or AC as well if it's not the top level
    return jtbox(jt, w);  // If top level, we have to make sure (<,'&')`  doesn't replace the left part with bare &
}

// Convert an AR to an A block.  w is a gerund that has been opened
// self is normally 0; if nonzero, we return a noun type ('0';<value) as is rather than returning value, and leave
// adv/conj ARs looking like nouns
A
jtfx(J jt, A w, A self) {
    A f, fs, g, h, p, q, *wv, y, *yv;
    C id;
    I m, n = 0;
    // if string, handle that special case (verb/primitive)
    if (LIT & AT(w)) return jtfxchar(jt, w, self);
    // otherwise, it had better be boxed with rank 0 or 1, and 1 or 2 atoms
    m = AN(w);  // m=#atoms
    ASSERT(BOX & AT(w), EVDOMAIN);
    ASSERT(1 >= AR(w), EVRANK);
    ASSERT(1 == m || 2 == m, EVLENGTH);
    wv = AAV(w);
    y  = wv[0];  // set wv->box pointers, y->first box
    // If the first box contains boxes, they are ARs - go expand them and save as fs
    // id will contains the type of the AR: 0=another AR, '0'=noun
    if (BOX & AT(y)) {
        RZ(fs = fx(y));
        id = 0;
    } else {
        RZ(y = jtvs(jt, y));
        ASSERT(id = spellin(AN(y), CAV(y)), EVSPELL);
    }
    if (1 < m) {
        y  = wv[1];
        n  = AN(y);
        yv = AAV(y);
        if (id == CNOUN) return self ? jtbox(jt, w) : y;
        ASSERT(1 >= AR(y), EVRANK);
        ASSERT(BOX & AT(y), EVDOMAIN);
    }
    switch (id) {
        case CHOOK:
        case CADVF: ASSERT(2 == n, EVLENGTH); return jthook(jt, fx(yv[0]), fx(yv[1]));
        case CFORK:
            ASSERT(3 == n, EVLENGTH);
            RZ(f = fx(yv[0]));
            ASSERT(AT(f) & VERB + NOUN, EVSYNTAX);
            RZ(g = fx(yv[1]));
            ASSERT(AT(g) & VERB, EVSYNTAX);
            RZ(h = fx(yv[2]));
            ASSERT(AT(h) & VERB, EVSYNTAX);
            return jtfolk(jt, f, g, h);
        default:
            if (id) fs = ds(id);
            ASSERT(fs && RHS & AT(fs), EVDOMAIN);
            if (!n) return fs;
            ASSERT(1 == n && ADV & AT(fs) || 2 == n && CONJ & AT(fs), EVLENGTH);
            if (0 < n) {
                RZ(p = fx(yv[0]));
                ASSERT(AT(p) & NOUN + VERB, EVDOMAIN);
            }
            if (1 < n) {
                RZ(q = fx(yv[1]));
                ASSERT(AT(q) & NOUN + VERB, EVDOMAIN);
            }
            return 1 == n ? df1(g, p, fs) : df2(g, p, q, fs);
    }
}

// Convert any DD (i. e. 9 : string or quoted string containing LF) found in a line to DD form for display
// w is a string block, and so is the result.
// If w is abandoned (which it is for recursive calls), the result is formed inplace over w
// if JTINPLACEA is set, make sure the result fits on one line for error display: stop copying if we hit LF and emit
// '...', and don't put spaces before/after the delimiters result is always incorpable
A
jtunDD(J jt, A w) {
    FPREFIP;
    I shortres = (I)jtinplace & JTINPLACEA;  // set for short, one-line result
    C *wv      = CAV(w);                     // start of word string
    // quick scan for 9, :, '; if not, return the input
    I scan;
    I qc9 = 0;
    for (scan = 0; scan < AN(w); ++scan) {
        I t = 0;
        t   = wv[scan] == '\'' ? 4 : t;
        t   = wv[scan] == ':' ? 2 : t;
        t   = wv[scan] == '9' ? 1 : t;
        qc9 |= t;
    }                // check for ' : 9 chars
    if (qc9 == 7) {  // if there is possibly a DD...
        // make input writable if it is not recursive; find words
        if (!((I)jtinplace & JTINPLACEW)) RZ(w = jtca(jt, w));
        wv = CAV(w);
        A wil;
        RZ(wil = jtwordil(jt, w));
        ASSERT(AM(wil) >= 0, EVOPENQ)
        // loop until no more DDs found
        I(*wilv)[2] = voidAV(wil);  // pointer to wordlist: (start,end+1) pairs
        I inx       = 0;            // next input character that has not been copied to the result
        I outx      = 0;            // next output position, built over the input
        I wilx      = 0;            // index in wil of next candidate DD
        while (1) {
            // find next 9 : if any.  We do some fancy skipping based on word length
            while (wilx <= AS(wil)[0] - 5) {
                if (wilv[wilx + 2][1] - wilv[wilx + 2][0] > 1)
                    wilx += 3;  // : too big, skip it
                else if (wv[wilv[wilx + 2][0]] != ':')
                    wilx += 1;  // not :, skip it
                else if (wilv[wilx + 1][1] - wilv[wilx + 1][0] > 1)
                    wilx += 2;  // 9 too big, skip it
                else if (wv[wilv[wilx + 1][0]] != '9')
                    wilx += 1;  // not :, skip it
                // the rest of the test is a formality
                else if (wilv[wilx][1] - wilv[wilx][0] > 1)
                    wilx += 1;  // ( too big, skip it
                else if (wv[wilv[wilx][0]] != '(')
                    wilx += 1;  // not (, skip it
                else if (wilv[wilx + 3][1] - wilv[wilx + 3][0] < 2)
                    wilx += 1;  // '' too small, skip it
                else if (wv[wilv[wilx + 3][0]] != '\'')
                    wilx += 1;  // not ', skip it
                else if (wilv[wilx + 4][1] - wilv[wilx + 4][0] > 1)
                    wilx += 1;  // ) too big, skip it
                else if (wv[wilv[wilx + 4][0]] != ')')
                    wilx += 1;  // not ', skip it
                else
                    break;  // if we matched, go handle it
            }
            wilx = wilx > AS(wil)[0] - 5 ? AS(wil)[0] : wilx;  // if no more DDs possible, pick entire rest of input
            // pack everything before the ( 9 : string ) down into the result.  We include space after
            // the last token in case it is needed for inflections
            // the ending index is the start pos of the new first word, but total length if we copy everything
            I endx = wilx == AS(wil)[0] ? AN(w) : wilv[wilx][0];
            // if we are about to move a LF character when we are limited to a single line, stop and output ...
            C currc = 0;
            if (inx != outx || shortres) {
                DQ(endx - inx, currc = wv[inx++]; if (shortres && currc == CLF) break;
                   wv[outx++] = currc;)  // copy in to end
            }
            if (wilx == AS(wil)[0]) break;  // break if no more DDs
            if (shortres && currc == CLF) {
                wv[outx++] = '.';
                wv[outx++] = '.';
                wv[outx++] = '.';
                break;
            }  // stop if we exceeded single line
            // install leading DD delimiter
            wv[outx++] = '{';
            wv[outx++] = '{';
            if (!shortres) wv[outx++] = ' ';
            // dequote the string and move it down into the result
            I startddx = outx;  // remember where the DD starts, because its length may be reduced
            inx        = wilv[wilx + 3][0] + 1;
            endx       = wilv[wilx + 3][1] - 1;
            while (inx < endx) {
                if (wv[inx] == '\'') ++inx;
                wv[outx++] = wv[inx++];
            }
            inx = wilv[wilx + 4][1];  // next input character will pick up after the final )
            // recur on the string to handle any 9 : it holds; update length when finished
            fauxblock(fauxw);
            A z;
            fauxvirtual(z, fauxw, w, 1, ACUC1);
            AK(z) += startddx;
            AN(z) = AS(z)[0] = outx - startddx;
            RZ(jtunDD((J)((I)jt | JTINPLACEW | shortres), z));
            // the recursion leaves the DD in place, but it may have become shorter if it too contained DDs (the {{ }}
            // overhead is always less than the ( 9 : '' ) overhead)
            outx = startddx + AN(z);
            // install trailing DD delimiter
            if (!shortres) wv[outx++] = ' ';
            wv[outx++] = '}';
            wv[outx++] = '}';
            // skip wordlist pointer to the next candidate
            wilx += 5;
        }
        // Install the length of the final result
        AN(w) = AS(w)[0] = outx;  // number of chars we transferred
    } else {
        // The input did not contain 9 : .  Keep it all,  UNLESS we need a short result.  In that case scan for LF,
        // and replace the LF with ... .
        if (shortres) {
            DO(
              AN(w), if (wv[i] == CLF) {
                  wv[i++] = '.';
                  if (i < AN(w)) {
                      wv[i++] = '.';
                      if (i < AN(w)) wv[i++] = '.';
                  }
                  AN(w) = AS(w)[0] = i;
                  break;
              })
        }
    }
    // 9 : string has been handled.  Any residual strings containing LF must be noun DDs, and must be represented as
    // such so that the result will be executable We just look for quoted strings containing LF, then replace with
    // {{)n,unquoted string}}
    if (!shortres) {     // if we can't add LF, don't expand strings
        I stringstartx;  // starting index of current ' string
        scan = 0;        // next position to examine
        while (1) {      // till all nounDDs emitted
            // look for next string
            for (stringstartx = scan; stringstartx < AN(w); ++stringstartx)
                if (wv[stringstartx] == '\'') break;
            if (stringstartx == AN(w)) break;  // if none, we're through
            C hasLF = 0;
            I numqu = 0;
            for (scan = stringstartx + 1; scan < AN(w); ++scan) {
                if (wv[scan] == CLF) hasLF = 1;  // see if nounDD needed
                if (wv[scan] == '\'') {
                    if (scan + 1 < AN(w) && wv[scan + 1] == '\'')
                        ++scan, ++numqu;
                    else
                        break;
                }  // exit loop at ondoubled quote
            }
            // we end with scan pointing to the final quote
            if (hasLF) {
                I finalLF = wv[scan - 1] == CLF;  // 1 if string ends with LF and thus must start with one
                // We must insert a nounDD.  We will allocate the string and copy header, unquoted middle, and trailer.
                // If the string doesn't end with LF, we use a one-line form; otherwise multiline
                // We could try to reduce number of copies, but this just isn't very common.  Unfortunately the nounDD
                // form is bigger than the quoted form length of the revised string is 6 ({{)n}}) plus
                // len+finalLF-2-numqu
                A neww;
                GATV0(neww, LIT, AN(w) + 6 - 2 - numqu + finalLF, 1);
                C *newwv = CAV(neww);
                memcpy(newwv, wv, stringstartx);
                newwv += stringstartx;  // pre-string, moving newwv to start of dequoted section
                memcpy(newwv, "{{)n\n", 5);
                newwv += 4 + finalLF;  // write the header of the nounDD, possibly starting with LF
                for (++stringstartx; stringstartx < scan; ++stringstartx) {  // skip the leading quote
                    *newwv++ = wv[stringstartx];
                    if (stringstartx + 1 < scan && wv[stringstartx + 1] == '\'') ++stringstartx;  // dedouble quote
                }        // move the quoted part
                ++scan;  // advance past the final quote
                memcpy(newwv, "}}", 2);
                newwv += 2;                              // trailer of nounDD
                memcpy(newwv, wv + scan, AN(w) - scan);  // the rest of the input
                scan = newwv - CAV(neww);  // adjust input pointer to the correct position in the new string
                w    = neww;
                wv   = CAV(w);  // pick up processing the modified string
            }
        }
    }
    // make result incorpable
    return jtincorp(jt, w);
}

static A
jtunparse1(J jt, CW *c, A x, I j, A y) {
    A q, z;
    C *s;
    I t;
    // for BBLOCK/TBLOCK types, convert the lines to displayable by displaying them as if for error messages, and
    // copying the result
    switch (t = c->type) {
        case CBBLOCK:
        case CBBLOCKEND:
        case CTBLOCK: RZ(z = jtunparse(jt, x)); break;
        case CASSERT:
            RZ(q = jtunparse(jt, x));
            GATV0(z, LIT, 8 + AN(q), 1);
            s = CAV(z);
            memcpy(s, "assert. ", 8L);
            memcpy(8 + s, CAV(q), AN(q));
            break;
        case CLABEL:
        case CGOTO: RZ(z = jtca(jt, AAV(x)[0])); break;
        case CFOR: RZ(z = c->n ? AAV(x)[0] : jtspellcon(jt, t)); break;
        default: RZ(z = jtspellcon(jt, t));
    }
    // if the CW we processed comes from the same source lime, append it and return the combination; ootherwise return
    // the new
    if (j == c->source) {
        GATV0(q, LIT, 1 + AN(y) + AN(z), 1);
        s = CAV(q);
        memcpy(s, CAV(y), AN(y));
        s += AN(y);
        *s++ = ' ';
        memcpy(s, CAV(z), AN(z));
        z = q;
    }
    return z;
} /* unparse a single line */

// unparse a definition, by going through the control words
// hv[0]->packed words, hv[1]->control words zv->pointer in array of boxes, indicating where to store the lines
// m is #CWs to process
// result points after the last line stored into zv
static A *
jtunparse1a(J jt, I m, A *hv, A *zv) {
    A *v, x, y;
    CW *u;
    I i, j, k;
    y = hv[0];
    v = AAV(y);  // v->word 0
    y = hv[1];
    u = (CW *)AV(y);  // u is running pointer through control words
    y = 0;
    j = k = -1;
    for (i = 0; i < m; ++i, ++u) {  // for each word
        RZ(
          x = jtunparse1(jt, 
            u, jtvec(jt, BOX, u->n, v + u->i), j, y));  // append new line to y or else return it as x if it is on a new line.
        k = u->source;
        if (j < k) {
            if (y) *zv++ = jtunDD(jt, y);
            DQ(k - j - 1, *zv++ = mtv;);
        }  // if we are about to move to a new line, save y and zap the surplus control words on the line to empties
        y = x;
        j = k;
    }
    if (y) *zv++ = jtunDD(jt, y);  // repeat to out last line
    DQ(k - j - 1, *zv++ = mtv;);
    return zv;
}

// w is a def.  Return unparsed form
A
jtunparsem(J jt, A a, A w) {
    A h, *hv, dc, ds, mc, ms, z, *zu, *zv;
    I dn, m, mn, n, p;
    V *wv;
    wv = VAV(w);
    h  = wv->fgh[2];
    hv = AAV(h);  // h[2][HN] is preparsed def
    mc = hv[1];
    ms = hv[2];
    m = mn = AN(mc);  // mc->control words ms->commented text
    dc     = hv[1 + HN];
    ds     = hv[2 + HN];
    n = dn = AN(dc);
    p      = n && (m || 3 == jti0(jt, wv->fgh[0]) ||
              VXOPR & wv->flag);  // p=2 valences present: dyad given, and  it's a verb or an operator referring to x
    if (jtequ(jt, mtv, hv[2]) && jtequ(jt, mtv, hv[2 + HN])) {
        // no comments: recover the original by unparsing
        if (m) mn = 1 + ((CW *)AV(mc) + m - 1)->source;
        if (n) dn = 1 + ((CW *)AV(dc) + n - 1)->source;
        GATV0(z, BOX, p + mn + dn, 1);
        zu = zv = AAV(z);
        RZ(zv = jtunparse1a(jt, m, hv, zv));
        if (p) RZ(*zv++ = chrcolon);
        RZ(zv = jtunparse1a(jt, n, hv + HN, zv));
        ASSERTSYS(AN(z) == zv - zu, "unparsem zn");
    } else {
        // commented text found.  Use it
        mn = AN(ms);
        dn = AN(ds);
        GATV0(z, BOX, p + mn + dn, 1);
        zv = AAV(z);
        DO(mn, *zv++ = jtunDD(jt, AAV(ms)[i]););
        if (p) RZ(*zv++ = chrcolon);
        DO(dn, *zv++ = jtunDD(jt, AAV(ds)[i]););
    }
    if (a == jfalse) {
        RZ(z = jtope(jt, z));
        if (1 == AR(z)) z = jttable(jt, z);
    }
    return z;
} /* convert h parameter for : definitions; open if a is 0 */

static A
jtxrep(J jt, A a, A w) {
    A h, *hv, *v, x, z, *zv;
    CW *u;
    I i, j, n, q[3], *s;
    V *wv;
    if (!(a && w)) return 0;
    RE(j = jti0(jt, a));
    ASSERT(1 == j || 2 == j, EVDOMAIN);
    j = 1 == j ? 0 : HN;
    ASSERT(AT(w) & VERB + ADV + CONJ, EVDOMAIN);
    wv = FAV(w);
    h  = wv->fgh[2];
    if (!(h && CCOLON == wv->id)) return jtreshape(jt, jtv2(jt, 0L, 3L), ds(CACE));
    hv = AAV(h);
    x  = hv[j];
    v  = AAV(x);
    x  = hv[1 + j];
    u  = (CW *)AV(x);
    n  = AN(x);
    GATV0(z, BOX, 3 * n, 2);
    s    = AS(z);
    s[0] = n;
    s[1] = 3;
    zv   = AAV(z);
    for (i = 0; i < n; ++i, ++u) {
        RZ(*zv++ = jtincorp(jt, jtsc(jt, i)));
        q[0] = u->type;
        q[1] = u->go;
        q[2] = u->source;
        RZ(*zv++ = jtincorp(jt, jtvec(jt, INT, 3L, q)));
        RZ(*zv++ = jtincorp(jt, jtunparse1(jt, u, jtvec(jt, BOX, u->n, v + u->i), -1L, 0L)));
    }
    return z;
} /* explicit representation -- h parameter for : definitions */

A
jtarx(J jt, A w) {
    F1RANK(0, jtarx, UNUSED_VALUE);
    return jtarep(jt, jtsymbrdlocknovalerr(jt, jtnfb(jt, w)));
}
A
jtdrx(J jt, A w) {
    F1RANK(0, jtdrx, UNUSED_VALUE);
    return jtdrep(jt, jtsymbrdlocknovalerr(jt, jtnfb(jt, w)));
}
A
jttrx(J jt, A w) {
    F1RANK(0, jttrx, UNUSED_VALUE);
    return jttrep(jt, jtsymbrdlocknovalerr(jt, jtnfb(jt, w)));
}
A
jtlrx(J jt, A w) {
    F1RANK(0, jtlrx, UNUSED_VALUE);
    return jtlrep(jt, jtsymbrdlocknovalerr(jt, jtnfb(jt, w)));
}
A
jtprx(J jt, A w) {
    F1RANK(0, jtprx, UNUSED_VALUE);
    return jtprep(jt, jtsymbrdlocknovalerr(jt, jtnfb(jt, w)));
}

A
jtxrx(J jt, A a, A w) {
    F2RANK(0, 0, jtxrx, UNUSED_VALUE);
    return jtxrep(jt, a, jtsymbrdlock(jt, jtnfb(jt, w)));
}  // 5!:7
