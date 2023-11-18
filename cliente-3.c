//ficheiro: cliente.c
/*
* O cliente envia uma mensagem ao servidor que depois a envia para o stdout
* Nome do socket fornecido pelo primeiro argumneto
* No cliente, mensagem a enviar ao sevidor fornecida pelo segundo argumento
* A comunicacao iniciada e terminada por cada invocacao do cliente
*
* ./servidor NomeSocket
* ./cliente NomeSocket mensagem
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>

int socket_fd; //descritor do socket


void fecha(){
	printf("O SERVIDOR FOi fechado!!!");
	close(socket_fd);
	exit(EXIT_FAILURE);
}

void *recebe_mensagem_servidor(void *arg){
	while(1){
	//ler a mensagem do socket enviada pelo servidor
	int id_socket_servidor = (intptr_t) arg;
	int tamanho;//variavel que guarda o tamanho da mensagem

        if(read(id_socket_servidor, &tamanho, sizeof(tamanho)) == 0)
            exit(EXIT_FAILURE);
        //printf("cliente: tamanho mensagem a receber do servidor... %d\n", tamanho);    

        char *mensagem = (char*) malloc(tamanho);

        read(id_socket_servidor, mensagem, tamanho);
		
		if(strcmp(mensagem,"exit")==0){
			fecha();
		}
		if(strcmp(mensagem,"USERS_ONLINE")==0){
			printf("\n\n\t %s",mensagem);

		}else{
			printf("\ncliente: A RECEBER %s\n", mensagem);
		}
		
		
	
        free(mensagem);
	}
}

void enviar_mensagem_servidor(int id_socket, char* mensagem){
    //escrever uma mensagem no socket do servidor
    //tamanho da mensagem contando o caracter final '\0'
    int tamanho = strlen(mensagem) + 1; 

    //printf("cliente: tamanho mensagem a enviar ao servidor... %d\n", tamanho);
    //escrever o numero de bytes da mensagem, incluindo o '\0'
    write(id_socket, &tamanho, sizeof(tamanho));

    printf("cliente: A ENVIAR: %s\n", mensagem);
    //escrever a mensagem 
    write(id_socket, mensagem, tamanho);
}

//o cliente vai receber por argumento o nome do socket e a mensagem 
int main(int argc, char *argv[]){
    //primeiro argumento e o nome do socket
    const char* const nome_socket = argv[1];
    //const char* const mensagem = argv[2]; //segundo argumento e a mensagem
    //int socket_fd; //descritor do socket
    struct sockaddr_un nome;//estrutura para o socket do unix
	pthread_t c_id_thread;

    while(1){//o cliente está sempre aberto, para várias utilizações
    	//criar o socket
    	socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);

    	//estabelecer o nome para o servidor
    	nome.sun_family = AF_LOCAL;
    	strcpy(nome.sun_path, nome_socket);

        char *mensagem = NULL; //inicialização que permite atribur valores através do getline
    	size_t tamanho = 0;
    	
		do{
        printf("Introduza start para começar:");
    	getline(&mensagem, &tamanho, stdin);
    	mensagem[strlen(mensagem)-1] = '\0';
		}while(strcmp(mensagem,"start")!=0);

		/* estabelecer o endereco (nome) a ligacao ao socket 
        	o cliente faz o pedido de ligacao ao socket servidor */
    	connect(socket_fd, (struct sockaddr*)&nome, SUN_LEN(&nome));
        pthread_create(&c_id_thread,NULL,recebe_mensagem_servidor,(void *)(intptr_t)socket_fd);
    	//ler a mensagem do teclado (standard input)
    	//char *mensagem;

    	//scanf("%s",mensagem);//lê a mensagem introduzida no teclado - termina com a mudança de linha
    	//o separador de string é o espaço (ou espaços)
    	//apenas lê a primeira palavra
    
    	//ler a linha completa de input - função getline
    	// stdin = standard input (teclado)

    	//enquanto a mensagem não for "sair" pode ser enviadas várias mensagens para o servidor
    	while(strcmp(mensagem,"end") != 0){
		//a mensagem != "sair\n" então continuamos a introduzir novas mensagens
    		//escrever a mensagem no socket - enviar a mensagem para o servidor

		//ler uma mensagem do socket enviada pelo servidor
		
		//recebe_mensagem_servidor(socket_fd);
	
		//introduzir uma nova mensagem
		printf("Introduza a sua mensagem:");
    		getline(&mensagem, &tamanho, stdin);
		mensagem[strlen(mensagem)-1] = '\0';
		enviar_mensagem_servidor(socket_fd,mensagem);
    		//printf("Mensagem introduzida pelo teclado -> %s\n", mensagem);
    	}
    	//quando chega aqui, significa que o utilizador quer sair (não introduzir mais mensagens)
     
    	close(socket_fd); //fechar o descritor do socket
    }

    return 0;
}