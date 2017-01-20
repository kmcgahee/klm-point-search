## Overview

This is my solution to the Churchill Nav point search challenge.  It uses a kd-tree, and while this works reasonable well for some seeds (~100x faster than reference), it falls apart for any queries containing large rectangles.  This is shown in the results below where the solution is ~3x slower than the reference.  After I completed this I found the thread on reddit, which led me to understand why (terrible caching).  Since this wasn't a submission for the actual competition, but rather an example of how I code for a job application, I didn't spend anymore time optimizing the code or switching to a better method.  

## Results

<pre>
--- Churchill Navigation Point Search Challenge ---
Point count  : 10000000
Query count  : 1000
Result count : 20
Random seed  : AF3A7C29-D5B21279-DF3A7C29-C56154AF-F4F293F2

Loading algorithms:
Loading reference.dll... Success.
Loading klm_point_search.dll... Success.
2 algorithms loaded.

Preparing 10000000 random points...done (2046.2630ms).
Preparing 1000 random queries...done (0.1290ms).

Testing algorithm #0 (reference.dll):
Robustness check...done.
Loading points...done (3601.5250ms).
Making queries...done (2084.7460ms, avg 2.0847ms/query).
Release points...done (memory used: 124MB).

Testing algorithm #1 (klm_point_search.dll):
Robustness check...done.
Loading points...done (9719.9500ms).
Making queries...done (9251.1420ms, avg 9.2511ms/query).
Release points...done (memory used: 459MB).

Comparing the results of algorithms:
#0 (reference.dll) and #1 (klm_point_search.dll): match.

Scoreboard:
#0: 2084.7460ms reference.dll
#1: 9251.1420ms klm_point_search.dll

Cleaning up resources...done.
</pre>