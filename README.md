# CPU cache use example

This repo provides different versions of the same code to show how cache use is
affected. The code basically iterate on a data structure (list or vector) and
sums its elements.

## Measuring

```sh
./report.sh list_sum ./list 10000000 random
./report.sh list_sum ./list 10000000 sequential
./report.sh vec_sum ./vector 10000000
```

It gives the following results on my machine (i7-8550U):
| | Cache references | Cache misses | L1d loads | L1d load misses | LLC loads | LLC load misses |
|-|------------------|--------------|-----------|-----------------|-----------|-----------------|
| Linked list random layout | 127554469 | 124181335 | 84326408 | 42786756 | 42232916 | 41598762 |
| Linked list sequential layout | 57062272 | 37403635 | 220071054 | 27501575 | 155093 | 88443 |
| Vector | 11198469 | 6243211 | 27471461 | 6895598 | 57675 | 33624 |
