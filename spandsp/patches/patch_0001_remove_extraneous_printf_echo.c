Index: src/echo.c
===================================================================
--- src/echo.c	(revision 2521)
+++ src/echo.c	(working copy)
@@ -475,7 +475,7 @@
                     {
                         if (ec->narrowband_score > 200)
                         {
-printf("Revert to %d at %d\n", (ec->tap_set + 1)%3, sample_no);
+//printf("Revert to %d at %d\n", (ec->tap_set + 1)%3, sample_no);
                             memcpy(ec->fir_taps16[ec->tap_set], ec->fir_taps16[3], ec->taps*sizeof(int16_t));
                             memcpy(ec->fir_taps16[(ec->tap_set - 1)%3], ec->fir_taps16[3], ec->taps*sizeof(int16_t));
                             for (i = 0;  i < ec->taps;  i++)
