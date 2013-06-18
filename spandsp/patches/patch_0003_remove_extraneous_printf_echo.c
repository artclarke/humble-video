Index: src/echo.c
===================================================================
--- src/echo.c	(revision 2523)
+++ src/echo.c	(working copy)
@@ -526,7 +526,7 @@
         {
             if (!ec->dtd_onset)
             {
-printf("Revert to %d at %d\n", (ec->tap_set + 1)%3, sample_no);
+//printf("Revert to %d at %d\n", (ec->tap_set + 1)%3, sample_no);
                 memcpy(ec->fir_taps16[ec->tap_set], ec->fir_taps16[(ec->tap_set + 1)%3], ec->taps*sizeof(int16_t));
                 memcpy(ec->fir_taps16[(ec->tap_set - 1)%3], ec->fir_taps16[(ec->tap_set + 1)%3], ec->taps*sizeof(int16_t));
                 for (i = 0;  i < ec->taps;  i++)
