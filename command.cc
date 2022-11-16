
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "command.h"
#include <time.h>

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}

	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;

	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_appendfile = 0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}

	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}

		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}
    if ( _appendfile ) {
		free( _appendfile );
	}
	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_appendfile = 0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");

	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );

}

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();
	pid_t executer;
	int status;
    int defaultin ;
    int defaultout;
    int defaulterr;
    int outfd;
    int file;
    int errorfile;
    int in;
    int f_dpipe[_numberOfSimpleCommands][2];
    for ( int i = 0; i < _numberOfSimpleCommands; i++ ){


        if(strcmp(_simpleCommands[0]->_arguments[ 0 ],"cd")==0)
        {
            if(_simpleCommands[0]->_arguments[ 1 ])
                chdir(_simpleCommands[0]->_arguments[ 1 ]);

            else {
                chdir("/home");
                 }
        break;
        }
         if(i==0){
            defaultin = dup( 0 );
            defaultout = dup( 1 );
            defaulterr = dup( 2 );

			}
       if(i<_numberOfSimpleCommands-1){
        if(strcmp(_simpleCommands[i+1]->_arguments[ 0 ],"grep")==0){
                pipe(f_dpipe[i]);
               dup2(f_dpipe[i][1],1);
               close(f_dpipe[i][1]);
          }
         }

        if(strcmp(_simpleCommands[i]->_arguments[ 0 ],"grep")==0){
        dup2(f_dpipe[i-1][0],0);
        close(f_dpipe[i-1][0]);
        if(i==_numberOfSimpleCommands-1)
        dup2(defaultout,1);
        }

        if(i==_numberOfSimpleCommands-1){
        if(_appendfile!=0 ){
            file=open(_appendfile,O_CREAT|O_APPEND|O_RDWR,00400|00200);
            dup2(file,1);
            close(file);
        }
         if (_outFile!=0){
            outfd = creat(_outFile, 0666);
            dup2(outfd, 1);
            close(outfd);
            }
        if (_inputFile!=0){
            in=open(_inputFile,O_RDWR,00400|00200);
            dup2(in,0);
            close(in);
        }
        if (_errFile!=0){
            errorfile = creat(_errFile, 0666);
            dup2(errorfile,2);
            close(errorfile);
        }

        }
        executer=fork();
        if (executer ==0 ){
            execvp(_simpleCommands[i]->_arguments[ 0 ],_simpleCommands[i]->_arguments);
        }
    }
    dup2(defaultin,0);
    dup2(defaultout,1);
    dup2(defaulterr,2);
    close( defaultin );
    close( defaultout );
    close( defaulterr );
	if(!_background)
        waitpid(executer,&status,0);
	if(!_background)
	{
        if (WIFEXITED(status))
        {
            clear();
            prompt();
        }
	}
	else{clear();
	prompt();}
	// Print new prompt

}
void child_graveyard(int sick_child){
	FILE*child_reporter;
    time_t timer=time(NULL);
	child_reporter=fopen("logfile.txt","a");
	fprintf(child_reporter,"a child is terminated %s\n" , ctime(&timer));
	fclose(child_reporter);
	}

// Shell implementation
void Signal_handler(int sig){
    printf(" you can't stop it\n");

}
void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;

int yyparse(void);

int
main()
{
	Command::_currentCommand.prompt();
	signal(SIGINT, &Signal_handler);
	signal(SIGCHLD, child_graveyard);
	yyparse();
	return 0;
}

