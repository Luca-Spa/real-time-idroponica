# Progetto idroponica
Questo progetto implementa un sistema di controllo distribuito per l'automazione di una coltivazione idroponica o serra.
Il sistema include un nodo edge da eseguire sulla scheda Raspberry Pi Pico e un nodo host. La comunicazione avviene tramite microros.
## Come eseguire il progetto
Per eseguire il progetto, è necessario avviare i processi in questo ordine:
1. Preparazione dell'Hardware (Pico)
* Compilare il firmware per Raspberry Pi Pico utilizzando CMake.
* Caricare il file `.uf2` generato sul Pico tenendo premuto il tasto BOOTSEL.
2. Avvio del micro-ROS Agent (Ponte Seriale)
* Aprire un terminale sul PC Host per instaurare la comunicazione seriale con il microcontrollore:
```bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0
```
* (Nota: la porta `/dev/ttyACM0` potrebbe variare a seconda del sistema operativo).
3. Avvio del Controller Host
* Eseguire il source del workspace ROS2 e avviare il nodo C++:
```bash
ros2 run <nome_del_tuo_pacchetto> pico_controller_node
```
