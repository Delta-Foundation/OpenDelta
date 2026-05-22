#!/bin/bash

function tty() {
    # путь к устройству tty
    TTY_PATH="/dev/tty1"

    # скорость baudrate
    BAUD_RATE=9600

    # проверяем на начилие даннного устройства по путю $TTY_PATH
    if [ ! -e "$TTY_PATH" ]; then
        echo "[err]: устройство $TTY_PATH не найдено."
        exit 1
    fi

    # устанавливаем agetty на устройстве
    agetty -L $BAUD_RATE $TTY_PATH
}

tty
