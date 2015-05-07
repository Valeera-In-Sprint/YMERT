# YMERT
My implementation of Minimum Error Training following David Chiang's cmert.

Version 1.0:

work well on Chiero, see run_dev_c2e_y.py for detail
about 1.0 bleu lower than state-of-art
run about 10x slower than state-of-art
anyway, a very nice start

Comment:
	The core idea of mert is to tune on one dimension and fix the others. 
So let's denote the scoring function as Y = \sum_i(w_i * f_i), so w_i are weights we want to tune,
f_i is the feature values, Y is the score for nbest ranking.
So if we focus on the jth dimension, the formula changes to Y= w_j * f_j + c.
We could see that f_j is the slope of the line, and c is the intercept of the line.
So for 1 sentences we have n lines, we first find the one (l1) with the biggest intercept, because it will surely have the topest intersection.
Then we find the one (l2) with the leftest intersection with l1. We save the intersection and substitue l1 with l2 to go on the process,
until there is no intersections. Noted that in the process, we always choose l2 with higher slope than l1, otherwise the intersection will not be the leftest.
So after get all the intersections for n sentences, we merge and sort them together, and calculate the bleu score at each interval,
find the lowest and pick the middle as the optimized weights for this dimension.
Then go on to the following dimensions.

And for mert, there are several tricky settings:
first is the number of initial points we have, the first one is set by user, the other are random starts
to prevent from getting into local minimum.
Second is the inner iteration number, we know that for n dimensions, we do n times line search, this is one iteration.
Normally we should be 5-10 iteration until the BLEU no longer increases.
The third is the external iteration, it is number we usually talks about. After each 5-10 inner iteration, the weights are fixed,
we do another round of MT decoding, to get new nbest list to merge with the old ones, then do MERT again.

