# OpenDelta. 

### Это бесплатная(до поры до времени) UNIX-like операционная система.
### Цель которой унифицировать и улучшить в целом UNIX системы.

------------------------------------------------------------------

# Версии утилит и программ в OpenDelta.
* OpenDelta: v0.1-a
* dltkernel: v0.0.9-a 
* dltsh: v0.0.14-d 

------------------------------------------------------------------

# Запуск и сборка из исходников.

## Запуск готового бинаря из релиза.
### Ставим git и виртулаку QMEU:

```
# arch
sudo pacman -S wget git qemu-system-x86 qemu-system-x86-firmware qemu-img qemu-base qemu-desktop
```

``` 
# FreeBSD (doas вместо sudo). clang компилятор и llvm инструменты установлены в системе по умолчанию.
doas pkg install wget qemu nasm git gmake
```

``` 
# void 
sudo xbps-install -S wget git qemu-system-x86_64 qemu-img qemu-base qemu-desktop 
```

``` 
# debian
sudo apt install wget qemu-system-x86 qemu-img qemu-base qemu-desktop

```

### Качаем файл и заупскаем:
``` 
wget https://github.com/Delta-Foundation/OpenDelta/releases/download/OpenDelta-unix-alpha-0.1/open-delta-unix.img
qemu-system-x86_64 -drive format=raw,file=open-delta-unix.img,media=disk,if=ide -d int,pcall,cpu_reset
```

## Сборка из исходников. 
### Если вы попробовали предыдущий пункт, то вам надо поставить лишь nasm, clang, llvm и lld.
### Можно скачать ветку `master-x86-64`, а можно и `master-i386`. Последняя пока не обновляется.
### Скачаем лучше `master-i386`.

``` 
# arch 
sudo pacman -S clang lld llvm nasm
```

``` 
# FreeBSD (doas вместо sudo). clang компилятор и llvm инструменты установлены в системе по умолчанию. gmake для сборки вместо BSD Make.
doas pkg install nasm gmake
```

``` 
# void 
sudo xbps-install -S clang nasm llvm ld.lld
```

``` 
# debian
sudo apt install nasm clang llvm lld
```

### Качаем репозиторий (ветка `master-i386`, но можно также `master-x86-64`):
`git clone -b master-i386 https://github.com/Delta-Foundation/OpenDelta.git`

### Переходим в папку я ядром, создаём две необходимые папки и запускаем сборку ядра:
`cd ~/OpenDelta/code/kernel && mkdir img/ && mkdir obj/ && make`

### На FreeBSD придётся ставить также gmake как было сказано выше.
`cd ~/OpenDelta/code/kernel && mkdir img/ && mkdir obj/ && gmake`

--------------------------------------------------------------------------

# Сборка dltsh.
### Бинарь запустить не получится (их там немало) так что собираем сами
### Устанавливаем компиляторы и библиотеки:
``` 
# arch 
sudo pacman -S clang ncurses rustc cargo
```

``` 
# FreeBSD
doas pkg install ncurses ncurses-devel rust rustup-init
```

``` 
# void 
sudo xbps-install -S clang ncurses ncurses-libs ncurses-devel ncurses-base rustc cargo
```

```
# debian
sudo apt install clang ncurses-base rustc cargo
```

### Если репоизторий уже скачан то остаётся перейти в папку `shell`:
`cd ~/OpenDelta/code/shell && mkdir bin/ && make & make run`

```
# FreeBSD
cd ~/OpenDelta/code/shell && mkdir bin/ && gmake & gmake run
```

### Длинная команда сверху переходит в нужную папку, создаёт папку для бинарей и запускает сборку.

------------------------------

# Авторы
* ### Главный разработчик и можно сказать создатель OpenDelta: это я - viadev0081.
### А больше и всё. никого нет.

-------------------------------

# Планы на будущее.
### У меня появилась идея сделать прошивку для редакции OpenDelta-ARM после установки которой вы сможете управлять вашим смартфоном как вам только влезет.
### И даже установить GrpaheneOS или HarmonyOS, Miui и прочие другое.
### Декларативный пакетный менеджер. Будет свой функциональный простенький язык, на котором вы с небольшимим усилиями настроить систему и всё что угодно.
### Возможность выбирать какие пакеты собрать из исходников и какие просто скачать бинарник. 
### Плюсы этого всего:
1. ### Вы сможете собрать пакет чётко под своё железо.
2. ### Больше плюсов нет.
3. Можно как раз таки не собирать пакет а просто скачать готовый бинарный файл.

### Минусы:
1. ### Сборка занимает много времени и ресурсов вашего ПК.
2. ### Если происходит небольшое обновление программы то пересборка всего будет также занимать крайне много нужного времени и ресурсов.
3. ### Надо знать про различные опции компилятора и его флаги. 

------------------------------

# Дополнительная информация.

### Наш логотип:
![logo](./doc/assets/new-opendelta.jpg)

### Сслыки:
[Telegram](https://t.me/open_delta_project)
[YouTube](https://www.youtube.com/channel/UC5ldSNpuwsSV98aIfRYWKFA)
