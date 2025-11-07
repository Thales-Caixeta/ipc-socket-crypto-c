# IPC com Sockets TCP + Mensagens Criptografadas (C)

**Aluno:** Thales Moura Machado Caixeta  
**Disciplina/Atividade:** Atividade_1_07_11_2025

## Visão Geral
Este projeto implementa comunicação **cliente-servidor** via **socket TCP** em C.  
As mensagens trafegam **criptografadas** com uma cifra de fluxo didática baseada em **xorshift32**.  
A criptografia é **simétrica**: a mesma função é usada para cifrar e decifrar.

> **Atenção:** Implementação didática. Não utilizar em produção.

## Protocolo
Cada mensagem é enviada como um “frame”:
```
[u32 big-endian length][payload]
```
O `payload` é a mensagem **cifrada** no fio. No servidor, ela é **decifrada** e exibida, e um **ACK cifrado** é retornado ao cliente.

## Arquivos
- `common.h` — utilitários de envio/recebimento (`send_all`, `recv_all`, frames).
- `crypto.h/.c` — cifra de fluxo com PRNG xorshift32; funções `crypto_xor_stream`, `ks_init`, etc.
- `server.c` — servidor TCP que recebe mensagem cifrada, decifra e responde com ACK cifrado.
- `client.c` — cliente TCP que lê uma mensagem do usuário, cifra e envia ao servidor.
- `Makefile` — build simples via `gcc`.

## Como compilar (Linux/WSL)
```bash
make
```
Isso gera `./server` e `./client`.

## Como executar
1. Em um terminal, subir o servidor (porta padrão **5050**):
   ```bash
   ./server 5050
   ```
2. Em outro terminal, rodar o cliente apontando para o servidor:
   ```bash
   ./client 127.0.0.1 5050
   ```
   Digite sua mensagem e pressione **ENTER**.

Por padrão, ambos usam a chave `"chave-secreta-demo"`.  
Você pode passar uma chave customizada **no cliente** como 4º argumento:
```bash
./client 127.0.0.1 5050 "minha-chave-top"
```
Se mudar a chave no cliente, altere a constante `key` em `server.c` para a mesma chave e recompile.

## Demonstração (exemplo)
Servidor:
```
[SERVIDOR] Escutando em 0.0.0.0:5050
[SERVIDOR] Cliente conectado.
[SERVIDOR] Mensagem recebida (decifrada): Olá prof! Isso está cifrado.
[SERVIDOR] ACK enviado (cifrado).
[SERVIDOR] Conexão encerrada.
```

Cliente:
```
[CLIENTE] Conectando em 127.0.0.1:5050
[CLIENTE] Digite a mensagem e pressione ENTER (máx 65535 chars):
Olá prof! Isso está cifrado.
[CLIENTE] Mensagem enviada (cifrada).
[CLIENTE] Resposta do servidor (decifrada): ACK: mensagem recebida com sucesso.
```

## Apresentação (tópicos para explicar ao professor)
- **IPC com socket TCP**: fluxo confiável, `bind/listen/accept` no servidor, `connect` no cliente.
- **Protocolo de framing**: `length (u32 big-endian) + payload`, evitando fragmentação/concatenação.
- **Criptografia caseira**: keystream gerado por `xorshift32` com semente derivada da chave (FNV-1a).
- **Simetria**: `crypto_xor_stream` serve para cifrar e decifrar.
- **Segurança x Didática**: propósito acadêmico; como evoluir para libs reais (OpenSSL, libsodium).

## Limpeza
```bash
make clean
```
# ipc-socket-crypto-c
