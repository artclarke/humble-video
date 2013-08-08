Index: src/echo.c
===================================================================
--- src/echo.c	(revision 2517)
+++ src/echo.c	(working copy)
@@ -427,7 +427,7 @@
 
     /* And the answer is..... */
     clean_rx = rx - echo_value;
-printf("echo is %" PRId32 "\n", echo_value);
+//printf("echo is %" PRId32 "\n", echo_value);
     /* That was the easy part. Now we need to adapt! */
     if (ec->nonupdate_dwell > 0)
         ec->nonupdate_dwell--;
@@ -602,7 +602,7 @@
         ec->cng = FALSE;
     }
 
-printf("Narrowband score %4d %5d at %d\n", ec->narrowband_score, score, sample_no);
+//printf("Narrowband score %4d %5d at %d\n", ec->narrowband_score, score, sample_no);
     /* Roll around the rolling buffer */
     if (ec->curr_pos <= 0)
         ec->curr_pos = ec->taps;
