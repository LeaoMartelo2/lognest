#define LOGNEST_DISABLE_DEBUG
#define LOGNEST_WARN_PREFIX "[warning]"
#define LOGNEST_IMPLEMENTATION
#define LOGNEST_DISABLE_TIMESTAMP
#include "../lognest.h"

int main(void) {

    lognest_init("exmple.log");

    lognest_trace("This is a trace message. Sample value: %d", 100);

    lognest_warn("This is a warn. %s", "Be careful!");

    lognest_error("This is an error message: %s", "Something went wrong!");

    lognest_debug("This message wont show up!");

    lognest_trace("This is a message without extra args!");

    return 0;
}





