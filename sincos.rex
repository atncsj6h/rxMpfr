#! /usr/bin/env rexx
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Copyright Enrico Sorichetti 2020 - 2021
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file BOOST_LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

parse source . . path . .
this = filespec( "N", path )

say "--" copies("- ",35 )
say "--" "'"this"'"

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
arch = ""

conf = config()
numeric digits conf[2]
say "--" "'"this"'" "precision" "bits("conf[1]")" "digits("conf[2]")"

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
prec = ""; units = "";
lo = ""; hi = ""; step = "";

do  argv over .syscargs
    optc = argv~left(argv~pos("=")-1 )
    optv = argv~substr(argv~pos("=")+1)
    select
        when ( abbrev( "-precision", optc, 2) ) then,
            prec = optv
        when ( abbrev( "-units", optc, 2) ) then,
            units = optv~left(1)~upper
        when ( abbrev( "-lower", optc, 2) ) then,
            lo = optv
        when ( abbrev( "-high", optc, 2) ) then,
            hi = optv
        when ( abbrev( "-step", optc, 2) ) then,
            step = optv
        otherwise ,
            NOP
    end
end

if  prec  = "" then prec = digits()
if  units = "" then units = "D"
if  lo    = "" then lo = 0
if  hi    = "" then hi = 360
if  step  = "" then step = 15

conf = config()

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
say "--" "'"this"'" "from("lo"); to("hi"); by("step")"
say;

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
times = ( hi - lo ) / step

width =  0

do  n = 0 to times
    x = lo+n*step

    __dsc = sincos( x , "dsin_sc."n, "dcos_sc."n )

    dsin.n = sin( x, prec );
    if length(dsin.n) > width then width = length(dsin.n);

    dcos.n = cos( x, prec );
    if length(dcos.n) > width then width = length(dcos.n);


    __rsc = sincos( radians(x), "rsin_sc."n, "rcos_sc."n, prec, "R")

    rsin.n = sin( radians(x), prec, "R");
    if length(rsin.n) > width then width = length(rsin.n);

    rcos.n = cos(radians(x), prec, "R");
    if length(rcos.n) > width then width = length(rcos.n);

end

width += 5

--
header = left("'"prec"'", 16, ".") left( ".degrees", width+4, ".") ,
    left( ".radians", width+4, ".") copies(".", width+4)


say header
do  n = 0 to times
    say "sin("right(lo+n*step,11)")" detail( dsin.n, dsin_sc.n, width )
end
say

say header
do  n = 0 to times
    say "cos("right(lo+n*step,11)")" detail( dcos.n, dcos_sc.n, width )
end
say

exit

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
detail: procedure
    use strict arg n, y, width
    if  n = "+infinity" | y = "+infinity" | ,
        n = "-infinity" | y = "-infinity" then ,
        return xedit(n,width) xedit(y,width)
    else ,
        return xedit(n,width) xedit(y,width) xedit(abs(n-y),width)
    return "oh! ... shit"

xedit:procedure
    use strict arg n, width
    s = left(n,1)
    if  s = "+" | s = "-" then ,
        buf = ""
    else ,
        buf = " "
    return left( buf || n, width+4 )

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::options   novalue condition

/*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*/
::requires "rxMpfr" library
