#pragma once

// The protected implementation is compiled under an internal name so the
// public export can add a low-latency cache without modifying TSD sources.
#define Set_Focus_Mode Set_Focus_Mode_Impl
