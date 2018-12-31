---
layout: default
title: lpc / prototypes
---

Version: master

- The function prototype:

The LPC function prototype is very similar to that of ANSI C. The
function prototype allows for better type checking and can serve as
a kind of 'forward' declaration.

return_type function_name(arg1_type arg1, arg2_type arg2, ...);

Also note that the arguments need not have names:

return_type function_name(arg1_type, arg2_type, ...);