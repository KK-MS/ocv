/*****************************************************************************
*
* A Drive Living Lab Confidential Proprietary
*
* Copyright (c) 2019 A Drive Living lab
* All Rights Reserved
*
*****************************************************************************
*
* THIS SOFTWARE IS PROVIDED BY A DRIVE LIVING LAB "AS IS" AND ANY EXPRESSED OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL A DRIVE LIVING LAB OR ITS CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
* IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************/

#include <stdio.h>
#include <string.h>
#include<winsock2.h>
#include <unistd.h>

#include "netrx.h"

using namespace std;

/*
 ** network_create
 **
 ** Create the network for listen the  Host client
 */
int network_create(netrx *ptrCliNet)
{
  WSADATA wsa;   
  char *serverIP = ptrCliNet->serverIP;
  int port       = ptrCliNet->port;
  int socket_desc_gt_bridge;

  struct sockaddr_in server, client;
  int length;
  char *message;

  memset(&server,0,sizeof(server));

  printf("\nInitialising Winsock...and b4 WSAStartup\n");

  fflush(stdout);

  memset((char *) &wsa, 0, sizeof(wsa));

  if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
      printf("Failed. Error Code : %d",WSAGetLastError());
      return 1;
  }

  printf("Initialised.\n");
  fflush(stdout);

  //Create a socket
  if((socket_desc_gt_bridge = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET)
  {
      printf("Could not create socket : %d" , WSAGetLastError());
  }
  printf("Socket created.\n");
  fflush(stdout);

  //Prepare the sockaddr_in structure
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(serverIP);
  server.sin_port = htons(port);

  //Bind
  if( bind(socket_desc_gt_bridge ,(struct sockaddr *)&server , sizeof(server)) == SOCKET_ERROR)
  {
      printf("Bind failed with error code : %d" , WSAGetLastError());
      exit(EXIT_FAILURE);
  }
  puts("Bind done");
  fflush(stdout);

  ptrCliNet->bind_sock_desc_gt_bridge = socket_desc_gt_bridge;

  return socket_desc_gt_bridge;
}

/*
 ** network_listen
 **
 ** Listen the Host client(windows) and connection established.
 **/
int network_listen(netrx *ptrCliNet)
{
  int socket_desc_gt_bridge = ptrCliNet->bind_sock_desc_gt_bridge;
  int socket_desc;
  struct sockaddr_in client;
  int length;
    
  memset(&client,0,sizeof(client));

  //Listen to incoming connections
  listen(socket_desc_gt_bridge, 3);

  //Accept and incoming connection
  puts("Waiting for incoming connections...");
  fflush(stdout);

  length = sizeof(client);

  socket_desc = accept(socket_desc_gt_bridge, (struct sockaddr *)&client, &length);

  puts("Connection accepted");
  fflush(stdout);

  ptrCliNet->sock_desc_gt_bridge = socket_desc;
  
  printf("\n GT_BRIDGE Server Created.:%d\n", socket_desc);

  return socket_desc;
}