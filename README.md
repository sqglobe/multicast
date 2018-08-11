# Multicast

Приложение использует широковещательную рассылку для **service descovery**.

## Приницип работы

      STARTED SERVER           SERVER               SERVER 
     ---------------          -------              --------
           |                    |                     |
           | --INIT_MESSAGE---> |                     |
           |                    |                     |
           | <-DESCOVER_MESSAGE |                     |
           |                    |                     |
           |                    |                     |
           | ------------------------INIT_MESSAGE---> |
           |                    |                     |
           | <-------------------DESCOVER_MESSAGE---- |


Когда приложение запускается, оно выаполняет широковещательную рассылку сообщения **INIT_MESSAGE**. 
Его назначение - сообщить всем уже запущенным экземлярам, что появился новый.
Все получившие это сообщение должны добавить в свой список серверов новый экземпяр и ответить ему, но уже 
через сообщение вида точка-точка. 

В процессе работы приложение выполняет рассылку всем известным ему экзумплярам 
через соединение точка-точка о том, что оно еще активно. 

Если какой либо сервер не подтвердил свою активность в течении некоторого 
промежутка, он считается не доступным и его адрес удаляется.

