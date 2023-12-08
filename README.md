# Sistemas Operativos
Programa escrito em C que consiste em dois executáveis, o monitor e o tracer, que formam um ambiente de monitorização cliente-servidor para a execução de programas. O monitor atua como um servidor, monitorizando a execução de programas e gerando relatórios de status. Já o tracer atua como um cliente que pode iniciar a execução de programas simples ou pipelines, interagindo com o monitor através de FIFO's.

## Monitor
É responsável por:
- Receber comandos dos clientes como "status" para obter informações sobre programas em execução ou PID para encerrar um programa;
- Manter uma lista de programas em execução, incluindo detalhes como o PID, o nome e o tempo de execução;
- Gerar relatórios de status que são enviados de volta para os clientes;
- Encerrar graciosamente em resposta ao sinal SIGINT (por exemplo, CTRL + C).

## Tracer
É capaz de:
- Executar programas simples ou pipelines de programas;
- Obter e imprimir a lista de programas em execução no servidor;
- Imprimir opções de utilização.

## Execução
Para executar programas e interagir com o sistema, utilize o tracer com o monitor aberto. Segue a seguir uma lista de comandos:

```bash
./monitor /path # Incia o servidor
```

```bash
./tracer -o # Exibe as opções de utilização
./tracer status # Obtém e imprime o status dos programas em execução
./tracer execute -u "program arg1 arg2" # Executa um programa simples
./tracer execute -p "program1 | program2" # Executa um pipeline de programas
```
