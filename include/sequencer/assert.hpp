#pragma once

#include <cassert>

#ifdef SEQUENCER_DISABLE_ASSERT
#define SEQUENCER_ASSERT( cond ) 123
#else
#define SEQUENCER_ASSERT( cond ) assert( cond );
#endif
