//
// Created by Aquib Nawaz on 22/09/24.
//

#ifndef SQLITEV2_MINUNIT_H
#define SQLITEV2_MINUNIT_H

#define mu_assert(message, test) do { if (!(test)) return message; } while (0)
#define mu_assert_iter(iter,message, test) do {  if (!(test)) {printf("%d ",iter); return message;} } while (0)
#define mu_run_test(test) do { const char *message = test(); tests_run++; \
                                if (message) return message; } while (0)
extern int tests_run;


#endif //SQLITEV2_MINUNIT_H
