# C++ MIPT
## Как работать с репозиторием
1. Создайте приватный репозиторий на GitHub.
2. Склонируйте себе этот репозиторий, настройте оригиналы и сделайте первый пуш
```console
git clone https://github.com/alexa0o/mipt-cpp-course.git
git remote rename origin baseOrigin
git remote add origin <your-repo>
git push -u origin main
```
3. Чтобы выгрузить изменения из основного репозитория выполните команды
```console
git fetch baseOrigin
git pull
```
4. Чтобы опубликовать результаты вашей работы в приватном репозитории
```console
git push -u origin <branch-name>
```
