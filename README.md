Come eseguire il progetto
Per testare l'intero ecosistema, è necessario avviare i processi in questo ordine esatto:
1. Preparazione dell'Hardware (Pico)
Compila il firmware per Raspberry Pi Pico utilizzando CMake.
Carica il file `.uf2` generato sul Pico tenendo premuto il tasto BOOTSEL.
2. Avvio del micro-ROS Agent (Ponte Seriale)
Apri un terminale sul PC Host per instaurare la comunicazione seriale con il microcontrollore:
```bash
ros2 run micro_ros_agent micro_ros_agent serial --dev /dev/ttyACM0
```
(Nota: la porta `/dev/ttyACM0` potrebbe variare a seconda del sistema operativo).
3. Avvio del Controller Host (Logica di Controllo)
Apri un secondo terminale, esegui il source del tuo workspace ROS2 e avvia il nodo C++:
```bash
ros2 run <nome_del_tuo_pacchetto> pico_controller_node
```
A questo punto, il sistema è completamente autonomo:
Nel terminale del controller vedrai i log di accensione/spegnimento di ventola e pompa in base alle oscillazioni dei sensori emulati.
Sulla scheda Raspberry Pi Pico vedrai il LED verde accendersi e spegnersi in risposta ai comandi inviati dal PC Host.