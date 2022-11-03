## Logic Classic - классическая игра на логику.

    Цель игры
Взломать код который задумал ~~игрок~~ машина.

     Базовая версия игры

  - Код состоит из 5 позиций
  - Количество используемых цветов 8
  - Цвета в коде не повторяются
  - На взлом дается 12 попыток

###

    Ход игры
      
 Игрок выставляет цветные фишки в замке с кодом. На каждом ходе компьютер дает подсказку в следующем формате:
 
 <img src="https://github.com/YaDanilamaster/LogicClassic/blob/master/img/info_full.png" alt="Полностью закрашенный кружок" width="30">  - полное совпадение цвета и позиции.
 
 <img src="https://github.com/YaDanilamaster/LogicClassic/blob/master/img/info_half.png" alt="Половина кружка закрашена" width="30">  - совпадение по цвету но он на неправильной позиции.
 
 
 <img src="https://github.com/YaDanilamaster/LogicClassic/blob/master/img/info_empty.png" alt="Пустой кружок" width="30"> - цвет отсутствует.

      Важно! Положение подсказок не соответствует положению цветов в коде.
 
 Например, если в
 подсказке указан один полностью закрашенный кружок и четыре полукружка - значит
 в коде совпали все цвета, но какой то один цвет стоит на своем месте, а остальные
 на неверных позициях. Какие именно - это и есть ЛОГИКА!

 Неоценимую помощь Вам окажет история попыток.
 
 В игре предусмотрены три уровня сложности:
 
 - Легко - положение подсказок соответсвтует положению в коде. Так очень легко понять какой цвет не на своемс мете а какого вообще нет.
 - Средний - это базовая версия игры. Придется попотеть.
 - Сложный - компьютер может загадывать один и тотже цвет дважды. Это вносит некоторую путаницу в процесс и усложняет процесс взлома.
 
## 
 
## О проекте

Проект не коммерческий, написан ради удовольствия и для своих детей, которые любят играть в эту игру, любят разгадывать код но для них слишком 
скучно загадывать код и ждать когда его взломают.


Проект написан на С++17 с использованием двух сторонних библиотек: SFML и Box2D.

### Архитектура программы

В общих чертах архитектура программы построена по принципу игровых сцен. Класс Scene предоставляет базовый набор методов и интерфейсов для работы с любой сценой (точка входа и выхода, обработка графических элементов, работа с камерой, системой событий и окном программы).

Управление игровым процессом происходит через систему событий. В проекте реализованы две системы событий:
- игровое событие - какое то действие, которое произошло на сцене или которое необходимо произвести на сцене
- событие ядра - движение мышки, изменение размера окна, щелчек мышки или нажатие кнопки, а также события из графических интерфейсов.

Добавление графических объектов на сцену происходит в шаблонном методе CreateGUInterface базового класса Scene

```
template<typename T, typename ...Args>

T* CreateGUInterface(const int priority, const bool canBeDeleted, Args&& ...);
```

Графические элементы представлены базовым классом GUInterface, который унаследован от класса Drawable библиотеки SFML. Таким образом, в наследниках GUInterface определены конкретные графические элементы которыми можно наполнять сцены.

Вот так выглядит добавление на сцену простого графического объекта - спрайт

```
Scene::CreateGUInterface<GUiSprite>(0, false, GetTexture((int)Texture::BackGround));
```

Или графического объекта Label. Объект Label - это графический аналог обычной ссылки, текст который поддерживает событие наведение мышки и клика.

```
CreateGUInterface<GUiLabel> ( 1, false, 
    Events::MouseDown | Events::MouseMoved, 
    GetFont((int)Font::Message), 
    (int)MenuItem::Check
);
```

События интерфейсов, равно как и любые события приложения обрабатываются шаблонным классом EventSystem. Классы подписываются на события EventSystem указывая ссылки на свои методы, которые необходимо вызывать при возникновении события.


Игровыми событиями на сцене управляет класс EventHandler. Сами игровые события реализованы в наследниках класса-интерфейса IEvents. Например, вот так будет выглядеть создание события "показать сообщение" длительностью 1,5 сек.

```
eventHandler->AddEvent(std::make_unique<ShowMessage>(message, 1500), EventType::Message);
```

Такая архитектура проекта позволяет лекго манипулировать игровыми объектами в процессе выполнения программы, управлять событиями на сцене, а также легко вводить новые объекты и события в игру.

### Сборка проекта
Для удобства сборки проекта в состав исходных файлов включены скомпилированные файлы сторонних библиотек SFML и Box2D, а также их заголовочные файлы. Собрать проект можно при помощи Cmake, файл CmakeList.txt прилагается.
