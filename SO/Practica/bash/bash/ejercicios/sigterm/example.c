/* Example of using sigaction() to setup a signal handler with 3 arguments
* including siginfo_t.
*/
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

static void hdl (int sig, siginfo_t *siginfo, void *context)
{
// Atención: Evitar el uso de printf en la función manejadora pues no se garantiza su
// correcto funcionamiento dentro de ellas. Usar write en su lugar. En este caso
// funciona porque es lo único que hace la función manejadora. En la man page de
// signal(2) hay una relación de funciones y llamadas que pueden ser llamadas desde la
// función manejadora sin problemas, printf no es una de ellas.
printf ("Sending PID: %ld, UID: %ld\n", (long)siginfo->si_pid, (long)siginfo->si_uid);
}

int main (int argc, char *argv[])
{

struct sigaction act;
memset (&act, '\0', sizeof(act));

// Use the sa_sigaction field because the handles has two additional parameters
act.sa_sigaction = &hdl;

//The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler.
act.sa_flags = SA_SIGINFO;

if (sigaction(SIGTERM, &act, NULL) < 0) {
	perror ("sigaction");
	return 1;
}

while (1)
	sleep (10);

return 0;
}
