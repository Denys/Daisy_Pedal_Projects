#pragma once

// Include before any target-dependent source. The no-macro configuration is the
// intentional pedal_125b default; two or more explicit selections are rejected.
#if (defined(VARIANT_125B) + defined(VARIANT_1590B) + defined(VARIANT_1590B_SMD) + \
     defined(VARIANT_TERRARIUM) + defined(VARIANT_FUNBOX)) > 1
#error "Exactly zero or one VARIANT_* macro is permitted"
#endif
