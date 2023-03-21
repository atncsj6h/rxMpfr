#! /usr/bin/env rexx
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
    Copyright Enrico Sorichetti 2020 - 2021
    Distributed under the Boost Software License, Version 1.0.
    (See accompanying file BOOST_LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
*/

parse source . . path . .
this = filespec( "N", path )

-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--  pi with 100 decimal places
--  3.1415926535 8979323846 2643383279 5028841971 6939937510
--    5820974944 5923078164 0628620899 8628034825 3421170679

--               1         2         3         4         5
--      12345678901234567890123456789012345678901234567890
pi =  3.14159265358979323846264338327950288419716939937510


do p over (17,21,36)
    numeric digits p+2
    say "--" "'"this"'" left(p,3) left(digits(),3) "pi+0." "'"pi+0."'"
    say "--" "'"this"'" left(p,3) left(digits(),3) "pi*0." "'"pi*1."'"
    say "--" "'"this"'" left(p,3) left(digits(),3) "pi+0." "'"pi/2."'"
    say "--"
end

exit


