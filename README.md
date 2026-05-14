# OpenDelta
Операционная система идущая на основе собственного ядра dltkernel и linux. Это и unix-like система которую разрабатываю только я и linux-distro который уже разрабатывается в команде.
_______________________________________________________________________________________________________

# Наш логотип
![logo](./doc/assets/new-opendelta.jpg)

# Версия dltsh:  v0.0.0.13-c
# Версия dltKernel: v0.0.8-p

________________________________________________________________________________________________________

# Сборка dltsh.
## Для начала установите необходимые зависимости:

``` sh
  # только для arch и arch-based
  sudo pacman -S clang
  sudo pacman -S make
  curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh 
```
``` sh
    # для void linux
    sudo xbps-install -S clang make curl
    curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Затем склонируйте этот репозиторий к себе в домашнюю папку:
` git clone https://github.com/deltadev-rsc/OpenDelta `

## Потом дайте права скрипту для сборки и запустите его(написан на make)
``` sh
    chmod +x ~/OpenDelta/shell/Makefile
    chmod +x ~/OpenDelta/scripts/shell/buildsh.sh
    make && make run
    # или же '~/OpenDelta/scripts/shell/buildsh.sh'
```

# Поздравляю вы запустили dltsh!'
_______________________________________________________________________________________________________

# Сборка dltKernel
## Установка зависимостей
``` sh
    # только для arch-based
    sudo pacman -S clang llvm lld llvm-objcopy
    sudo pacman -S nasm
    sudo pacman -S make
    sudo pacman -S qemu-system-i386
```

``` sh
    # для void linux
    sudo xbps-install -S clang llvm lld 
    sudo xbps-install -S llvm-objcopy
    sudo xbps-install -S nasm make
    sudo xbps-install -S qemu-system-i386
```

## Клонирование репозитория
` git clone https://github.com/deltadev-rsc/OpenDelta `

## Момент сборки

> ### Предупреждение!
> ### не советую запускать если после того как вы прописали в терминале qemu-system-i386 не появилось новое окно.


``` sh
    cd ~/OpenDelta/src
    make
    # запуск по команде: make run
```

_______________________________________________________________________________________________________

# Авторы
## Ведущий и главный разработчик: я - deltadev-rsc
## Sharkobaton: Второй разраб на котором лежит линуксовая релакция опендельты.
## KOER31: Третий(не лишний) разработчик который напишет DE ну или по русски то графическую оболочку. А на данный момент делает эмулятор терминала.
## Arsenuihdjd: создатель сайта для нашей ОС(пока его нет).

_______________________________________________________________________________________________________

# Планы на будущее.
## У меня появилась идея сделать прошивку для редакции OpenDelta-ARM после установки которой вы сможете управлять вашим смартфоном как вам только влезет и даже установить GrpaheneOS или HarmonyOS
_______________________________________________________________________________________________________

# дополнительные ссылки.
## deltadev-rsc это [DeltaDevSys](https://github.com/DeltaDevSys), но я не смог на том аккаунте включить 2FA, а на этом smog.
[TG](https://t.me/open_delta_project)
_______________________________________________________________________________________________________
