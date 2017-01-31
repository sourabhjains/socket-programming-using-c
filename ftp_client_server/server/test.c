#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
   int day, year;
   char weekday[20], dtm[100];

   strcpy( dtm, "Saturday@March" );
   sscanf( dtm, "%s %s", weekday, weekday);

   printf("%s\n", weekday );
    
   return(0);
}
