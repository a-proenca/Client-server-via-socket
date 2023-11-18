//ficheiro: servidor.c
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
#include <stdint.h>
#include <signal.h>
#include <stdbool.h>

int flag=0;
int count=0;
int c=0;
int *client_array;
int socket_fd; //descritor do socket
const char* nome;




void enviar_mensagem_cliente(int id_socket, char* mensagem){
    //escrever uma mensagem no socket do cliente
    //tamanho da mensagem contando o caracter final '\0'
    int tamanho = strlen(mensagem) + 1; 

    //printf("servidor: tamanho mensagem a enviar ao cliente... %d\n", tamanho);
    //escrever o numero de bytes da mensagem, incluindo o '\0'
    write(id_socket, &tamanho, sizeof(tamanho));

    //printf("servidor: a enviar mensagem ao cliente... %s\n", mensagem);
    //escrever a mensagem 
    write(id_socket, mensagem, tamanho);
}

void trata_sinal(){
    printf("O utilizador interrompeu o servidor!!!\n O servidor ira fechar...");
    sleep(1);
    for(int i=0; i<count; i++){
     enviar_mensagem_cliente(client_array[i], "exit");
    }
    close(socket_fd);
    unlink(nome);
    exit(EXIT_FAILURE);
}

int recebe_mensagem_cliente(int id_socket_cliente){
   while(1){
	//ler a mensagem do socket enviada pelo cliente
	int tamanho;//variavel que guarda o tamanho da mensagem
    
        if(read(id_socket_cliente, &tamanho, sizeof(tamanho)) == 0)
            return 0;
        //printf("servidor: tamanho mensagem a receber do cliente... %d\n", tamanho);    

        char *mensagem = (char*) malloc(tamanho);

        read(id_socket_cliente, mensagem, tamanho);
        printf("servidor: A RECEBER DO CLIENTE ID:%d\nMENSAGEM: %s\n",id_socket_cliente,mensagem);

        if(strcmp(mensagem,"users")==0){
            enviar_mensagem_cliente(id_socket_cliente,"USERS_ONLINE");
            for(int i=0 ; i<count; i++){
            char p[20];
            sprintf(p, "USER --> %d",client_array[i]);
            enviar_mensagem_cliente(id_socket_cliente,p);
            }
        
        }
    if(strcmp(mensagem,"end")==0){
        for(int i=0; i<count; i++){
           if(client_array[i]==id_socket_cliente)
             client_array[i]=-1; 
        }
    }
    if(strcmp(mensagem,"end")!=0 || strcmp(mensagem,"users")!=0){
     c=c+1;
	//o servidor já tem a mensagem do cliente
	//enviar a mesma mensagem para o mesmo cliente

    if(c%2==0){
        for(int i=0; i<count; i++){
            if(client_array[i] % 2 == 0){
                enviar_mensagem_cliente(client_array[i],mensagem);
            }
        }
    }else{
        for(int i=0; i<count; i++){
           if(client_array[i] % 2 != 0){
               enviar_mensagem_cliente(client_array[i],mensagem);
           }
        }
    }
    
	//enviar_mensagem_cliente(id_socket_cliente,mensagem);
	
        //quit = strcmp(mensagem, "quit");
        //printf("quit = %d\n", quit);
        free(mensagem);
    }
   }
}


void *conversa(void *arg){
   int cliente_socket_fd = (intptr_t) arg;
   if(cliente_socket_fd==4){
            client_array = (int *)malloc(sizeof(int)*1);
            count++;
            if(client_array == NULL){
                printf("Falha na alocação de memoria");
                exit(EXIT_FAILURE);
            }
        }else
        {
            count++;
            client_array = (int *)realloc(client_array,sizeof(int)*count);
            
        }
   //ler a mensagem enviada pelo cliente

        client_array[count-1]=cliente_socket_fd;

    
   recebe_mensagem_cliente(cliente_socket_fd);

   //fechar a ligacao - fechar o socket do cliente
   close(cliente_socket_fd);
   pthread_exit(NULL);
}

int main(int argc, char *argv[]){
    //nome do socket fornecido por argumento
    const char* const nome_socket = argv[1];
    nome = argv[1];
    //int socket_fd; //descritor do socket
    struct sockaddr_un nome; //estrutura para o socket unix

    signal(SIGINT,trata_sinal);
    //criar um socket local para comunicacao de streams
    socket_fd = socket(PF_LOCAL, SOCK_STREAM, 0);

    /* definir este processo como sendo o servidor,
        preencher a estrutura para o socket */
    nome.sun_family = AF_LOCAL;
    strcpy(nome.sun_path, nome_socket);

    //bind - vincular o socket com o endereco (nome)
    bind(socket_fd, (struct sockaddr*) &nome, SUN_LEN(&nome));

    //configurar o socket para aceitar ligacoes e fica a aguardar por pedidos
    listen(socket_fd, 5);

    /*enquanto o cliente nao enviar a mensagem "quit", 
        aceita as ligacoes e recebe mensagens */
    //int quit = 1;
    pthread_t id_thread;
    do{
        //estrutura para o socket unix do cliente
        struct sockaddr_un nome_cliente; 
        socklen_t cliente_nome_len; //tamanho do nome do socket do cliente
        int cliente_socket_fd; //descritor do cliente

        //aceitar a ligacao e retorna um descritor para o socket cliente
        cliente_socket_fd = accept(socket_fd, (struct sockaddr*) &nome_cliente, 
        &cliente_nome_len);

        //printf("cliente_socket_fd = %d\n", cliente_socket_fd);
	    //a conversa vai ser controlada por uma thread
        pthread_create(&id_thread,NULL,conversa,(void *)(intptr_t)cliente_socket_fd);
        //signal(SIGINT,trata_sinal);
    
    //} while (quit != 0);
    } while(1); //ciclo infinito - o servidor está a ser executado por tempo indeterminado
    //termina quando o cliente envia a mensagem "quit"
    //remover o descritor do socket - fechar o socket e desvincular o nome
    close(socket_fd);
    unlink(nome_socket);
    return 0;
}
