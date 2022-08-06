#!/bin/sh

#this script for update RoninEngine to latest version
echo "Adding remote ronin-engine -> engine"
git remote add -f engine https://github.com/badcast/ronin-engine.git

echo "removing existence sources"
rm -rf ./RoninEngine

git subtree add --prefix=RoninEngine https://github.com/badcast/ronin-engine.git master

mkdir -p ./RoninEngine
git subtree pull --prefix=RoninEngine/ engine master
git pull -v engine master

#set RoninEngine sources as read-only
find ./RoninEngine/src/ronin/ -type f -exec chmod -R 444 {} \;
