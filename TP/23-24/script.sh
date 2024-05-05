#!/bin/bash

# Verifica se o número correto de argumentos foi fornecido
if [ $# -lt 1 ]; then
    echo "Uso: $0 <número de tarefas>"
    exit 1
fi

# Pasta onde o output das tarefas será armazenado
output_folder="output"

# Número de tarefas que podem ser executadas em paralelo
parallel_tasks=2

# Número de tarefas a serem enviadas ao orchestrator
num_tasks=$1

# Comando para enviar uma tarefa ao orchestrator
send_task_command="./bin/client execute 1000 -u"

# Lista de comandos a serem executados
commands=("ls -l 1000" "neofetch 500" "date" "uptime" "df -h")

# Loop para enviar as tarefas ao orchestrator
for ((i=0; i<$num_tasks; i++)); do
    # Seleciona um comando aleatório da lista
    idx=$((RANDOM % ${#commands[@]}))
    task_command="${commands[$idx]}"

    # Envie a tarefa ao orchestrator
    $send_task_command "$task_command" &
    
    # Espere um segundo antes de enviar a próxima tarefa
    sleep 1
done

# Aguarde até que todas as tarefas sejam concluídas
wait

echo "Todas as tarefas foram enviadas ao orchestrator."
