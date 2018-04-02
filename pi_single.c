#include <stdio.h>
#include <time.h>
#include <sys/time.h>     // gethrvtime
//2 billion terms -> ~8 digits of precision
#define N_TERMS 2000000000LL
/* Estimate Pi using Leibniz's sum using N terms */
double pi(long long N) {
    /*Initialize sum */
    double sum = 0.0;
    /* Approximate pi/4 */
    for (long long  k = 0; k < N; ++k) {
        sum += (k % 2 ? -1.0 : 1.0) / (2 * k + 1);
    }
    /* Multiply by four to approximate pi */
    return 4 * sum;
}
int main()
{
    /* Begin measuring */
    hrtime_t start, end;
    start = gethrvtime();

    /* Run estimate to N terms */
    double pi_estimate = pi(N_TERMS);

    /* Report time and estimate */
    end = gethrvtime();
    printf("Time: %lld nsec\n",end - start);
    printf("Leibniz Pi estimate with %lld terms: \n %.9lf\n"
            , N_TERMS, pi_estimate);
    return 0;
}
