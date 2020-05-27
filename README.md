# Проект "Монополия" 
## О проекте
### Основная идея
"Монополия" — экономическая и стратегическая настольная игра для 2-6 человек.

Цель игры — рационально используя стартовый капитал, добиться банкротства других игроков.

Подробнее узнать об игре, а также ознакомиться с правилами, вы можете [здесь](https://www.keft.ru/help/mnp/rules).

### Проект
Данный проект - реализация Монополии в виде компьютерной игры.

Текущая версия проекта представляет собой приложение, написанное на **SFML**. 

Реализованы: 
* Игра по сети
* Выбор количества игроков (от 2 до 6)
* Вся логика оригинальной игры, включая торговлю между игроками и аукционы

## Установка
Операционная система: **OS Linux** или **MacOS**
### Необходимые библеотеки
* SFML 2.5.1
  * Linux
  ```shell script
  $ sudo apt-get install libsfml-dev
  ```
  * MacOS
  ```shell script
  brew install sfml
  ```
* sockpp commit 93855d5 (добавлена как submodule)
  ```shell script
  $ git submodule update --init --recursive
  ```
* [protobuf 3.11.4](https://github.com/protocolbuffers/protobuf)
  ```shell script
  $ cd protobuf
  $ ./configure
  $ make
  $ sudo make install
  $ sudo ldconfig
  ```

### Сборка
```shell script
$ git clone https://github.com/gt22/hse-proj-monopoly
$ cd hse-proj-monopoly
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Использование

Для запуска: 

находясь в директории `build` запустить проект

 ```shell script
      $ ./monopoly
  ```

Взаимодействие с игрой:

Взаимодействие с игрой происходит при помощи левой и правой кнопок мыши.
Также в моменты торговли между игроками и подключения по сети требуется ввести число с клавиатуры.
Чтобы начать игру, сначала нужно выбрать игроков с помощью кнопки `add player`, а потом нажать `start game`.
 
Во время игры слева находится панель взаимодействия. Подсвечиваются активные кнопки, отвечающие за доступные в данный момент текущему игроку действия.
При наведении мышкой на кнопки, в левом нижнем углу появляется информация о том, что произойдет при нажатии на эту кнопку.
Чтобы узнать информацию о клетке поля, нужно нажать на правую кнопку мыши. Информация появится в правом нижнем углу.

## Авторы
* https://github.com/gt22
* https://github.com/xbreathoflife
* https://github.com/AlinaUsh
