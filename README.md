# Райтап по практическому заданию

## Введение

В качестве тестового задания на кафедру из двух вариантов я выбрал второй - задание на сокеты, развёрнутые в эмуляции локальной сети.

Думаю, стоит начать с **мотивации** выбора:

Задание на ML показалось более интересным, но слишком незнакомым для меня - нужно было разобраться в очень большом количестве теории, и в голове не было конкретного плана действий. С сокетами по-другому: тоже интересное задание, но при этом сразу было понятно что делать и в каком порядке (в том числе что придётся гуглить и так далее)

## Оглавление
1) [Развёртка виртуальной машины](#вм)
2) [Настройка локальной сети](#лс)
3) [Основная программа](#оп)
4) [Тестирование](#тест)
5) [Wireshark](#wireshark)
6) [Дополнение](#доп)  

<a name="вм"></a>
## Развёртка виртуальной машины (virtualbox)

В качестве операционной системы для виртуальных машин выбрал **Ubuntu server 24.10 (no LTS)** для уменьшения нагрузки на пк и выделил практически минимальные ресурсы (по одному ядру процессора, 2ГБ оперативной памяти и виртуальные диски по 4ГБ)

Впоследствие оказалось, что следовало бы выбирать систему с графическим интерфесом (хотя бы минимальным) для нормальной работы wireshark, но в итоге это не стало проблемой.

Проблем в процессе не возникло, думаю, рассказывать в этом пункте больше не о чем

<a name="лс"></a>
## Настройка локальной сети

Для настройки локальной сети нужно было выполнить 2 шага:

- Найстроить адаптер в каждой из виртуальной машины
- Вручную прописать конфиг в netplan


для шага 1 необходимо открыть настройки сети в virtualbox у виртуальной машины, вырбать любой свободный адаптер и поставить режим intertal network

___
![adapter settings](https://github.com/cdeclcdecl/nettask/blob/master/pics/adapter.png)
___

После настройки адаптера прописываем в виртуалке
```bash
ip addr show
```
И получаем следующий результат:

___
![netdevs](https://github.com/cdeclcdecl/nettask/blob/master/pics/ipaddr.png)
___

В нём запоминаем название нового сетевого устройства (в моём случае - **enp0s8**) 

для шага 2:
```bash
sudo nano /etc/netplan/01-netcfg.yaml
```
И заполняем файл следующим образом

___
![netcfg](https://github.com/cdeclcdecl/nettask/blob/master/pics/config.png)
___

Здесь **enp0s8** - название устройства, **192.168.10.10** - ip первой виртуалки

Далее,
```bash
sudo netplan apply
```
и аналогичные действия на второй виртуалке (только меняем ip в конфиге - в моём случае на 192.168.10.11)

Проеряем на первой виртуалке
```bash
ping 192.168.10.11
```
Работает!

<a name="оп"></a>
## Основная программа

Файл с основной программой лежит в репозитории (2ip.c) (https://github.com/cdeclcdecl/nettask/blob/master/2ip.c)

Дополнительно добавил:
- отключение клиента по таймауту 5 секунд (если сервер, например, недоступен)
- флаг -h, который выводит справку помощи в stdout

Остальное реализовано по заданию

<a name="тест"></a>
## Тестирование

Для тестирования дополнительно реализовал программу xref.c (https://github.com/cdeclcdecl/nettask/blob/master/xref.c) - обёртка основного кода в fork-exec, для избежания монополизации shell'a выполнением программы. Таким образом можно запускать сразу несколько серверных процессов, а также перекрёстно подключаться в режиме клиента, когда серверы запущены на обоих виртуалках.

___
![testing](https://github.com/cdeclcdecl/nettask/blob/master/pics/testing.png)
___

В целом, проблем не возникло, всё работало практически сразу

Помимо этого попробовал пофаззить программу: ничего толкового не нашлось, единственное - зависание при обращении клиента к нерабочему серверу (неправильный айпи или порт) пофиксил, как писал ранее через аларм. На странные вфодные данные программа реагирует нормально, непредвиденных ошибок не нашёл

<a name="wireshark"></a>
## Wireshark

Первая и единственная проблема появилась на этом этапе. Как я писал ранее, на виртуалках установлен ubuntu server, т.е ос без графической оболочки. Изначальная мысль была в том, чтобы развернуть wireshark на локалке, но я не учёт тот факт, что в режиме Internal network сетевое взаимодействие происходит без участия гипервизора (то есть хост пк). Таким образом, нужно запускать wireshark внутри виртуалки, однако без графического интерфейса он отказывается работать.

Появляется 2 варинта:
- Загрузить какую-то простейшую графическую оболочку на виртуалку
- Использовать консольный аддон вайршарка - tshark

Из этих вариантов более осмысленным мне показался второй, поэтому дальнейшие действия я производил с помощью tshark

Порядок действий:
1. На первой виртуалке устанавливаем wireshark и tshark
2. Запускаем сервер на порту 2025 (для примера)
3.
   ```bash
   sudo tshark -D   # Видим список устройств, выбираем нужное (enp0s8)
   sudo thsark -i enp0s8 -f "udp port 2025" -w tr.pcap
   # Читаем udp трафик на порту 2025 и записываем результат в файл tr.pcap (т.е. трафик уже отфильтрован)
   ```
5. Переносим tr.pcap на локалку и открываем wireshark в графической оболочке

___
![wsh1](https://github.com/cdeclcdecl/nettask/blob/master/pics/sh1.png)
![wsh2](https://github.com/cdeclcdecl/nettask/blob/master/pics/sh2.png)
___

Как видно на скрине, переданные пакеты корректно отображаются

И, насколько я понимаю, задание выполнено

<a name="доп"></a>
## Дополнение

Проект, как и сказано в задании, реализован с использованием git, но до меня слишком поздно дошло, что переносить файлы с виртуалок на локалку и наоборот можно с помощью git clone, поэтому для переноса файлов в процессе выполнения я использовал ssh подключение к виртуалкам и shared folders из virtualbox
