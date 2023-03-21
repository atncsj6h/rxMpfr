#! /usr/bin/env rexx
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--  Copyright Enrico Sorichetti 2023 - 2023
--  Distributed under the Boost Software License, Version 1.0.
--  (See accompanying file BOOST_LICENSE_1_0.txt or copy at
--  http://www.boost.org/LICENSE_1_0.txt)
--

parse source . . path . .
this = filespec( "N", path )

say "--" copies("- ",35 )
say "--" "'"this"'"
say;

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
conf = config()
numeric digits conf[2]

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
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

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
say "--" "'"this"'" "from("lo"); to("hi"); by("step")"
say;

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
times = ( hi - lo ) / step

width =  0

do  n = 0 to times

  x = lo+n*step

  dsin.n = sin( x, prec );
  if length(dsin.n) > width then width = length(dsin.n);

  dcos.n = cos( x, prec );
  if length(dcos.n) > width then width = length(dcos.n);

  dtan.n = tan( x, prec );
  if length(dtan.n) > width then width = length(dtan.n);

  dcot.n = cot( x, prec );
  if length(dcot.n) > width then width = length(dcot.n);

  rsin.n = sin(radians(x), prec, "R");
  if length(rsin.n) > width then width = length(rsin.n);

  rcos.n = cos(radians(x), prec, "R");
  if length(rcos.n) > width then width = length(rcos.n);

  rtan.n = tan(radians(x), prec, "R");
  if length(rtan.n) > width then width = length(rtan.n);

  rcot.n = cot(radians(x), prec, "R");
  if length(rcot.n) > width then width = length(rcot.n);

end

width += 2

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
header = left("'"prec"'", 16, ".") left( ".degrees", width, ".") ,
  left( ".radians", width, ".") copies(".", width)


say header
do  n = 0 to times
  say "sin("right(lo+n*step,11)")" detail( dsin.n, rsin.n, width )
end
say

say header
do  n = 0 to times
  say "cos("right(lo+n*step,11)")" detail( dcos.n, rcos.n, width )
end
say

say header
do  n = 0 to times
  say "tan("right(lo+n*step,11)")" detail( dtan.n, rtan.n, width )
end
say

say header
do  n = 0 to times
  say "cot("right(lo+n*step,11)")" detail( dcot.n, rcot.n, width )
end
say

exit

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
detail: procedure
  use strict arg n, y, width
  if  n = "+infinity" | y = "+infinity" | ,
    n = "-infinity" | y = "-infinity" then ,
    return xedit(n,width) xedit(y,width)
  else ,
    return xedit(n,width) xedit(y,width) xedit(abs(n-y),width)
  return "oh! ... shit"

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
xedit:procedure
  use strict arg n, width
  s = left(n,1)
  if  s = "+" | s = "-" then ,
    buf = ""
  else ,
    buf = " "
  return left( buf || n, width )

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
::options   novalue condition

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
::requires "rxmpfr" library
