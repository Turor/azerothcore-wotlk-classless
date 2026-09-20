# ![logo](https://raw.githubusercontent.com/azerothcore/azerothcore.github.io/master/images/logo-github.png) AzerothCore

[![Contributor Covenant](https://img.shields.io/badge/Contributor%20Covenant-2.1-4baaaa.svg)](CODE_OF_CONDUCT.md)
[![CodeFactor](https://www.codefactor.io/repository/github/azerothcore/azerothcore-wotlk/badge)](https://www.codefactor.io/repository/github/azerothcore/azerothcore-wotlk)
[![StackOverflow](http://img.shields.io/badge/stackoverflow-azerothcore-blue.svg?logo=stackoverflow)](https://stackoverflow.com/questions/tagged/azerothcore?sort=newest "Ask / browse questions here")
[![Discord](https://img.shields.io/discord/217589275766685707?logo=discord&logoColor=white)](https://discord.gg/gkt4y2x "Our community hub on Discord")
[![Bounties on BountyHub](https://img.shields.io/badge/Bounties-on%20BountyHub-yellow)](https://www.bountyhub.dev/bounties?repo=azerothcore)

## Classless fork (Turoran)

This is [Turor/azerothcore-wotlk-classless](https://github.com/Turor/azerothcore-wotlk-classless): AzerothCore 3.3.5a plus the C++ hooks the classless module needs. It is **not** a drop-in for stock [azerothcore/azerothcore-wotlk](https://github.com/azerothcore/azerothcore-wotlk) `master`.

The classless module, ALE Lua engine, AH bot, dungeon master, and starter-guild are **git submodules** recorded on this repo’s `master`. Clone this fork; do not copy `mod-classless` into a vanilla AzerothCore tree.

**Branches:** `master` is what you deploy. `develop` is fast-forwarded to `master` after promotion, so they should be the same SHA. Daily C++ work lands on `develop`, then `master` is fast-forwarded.

**Module:** [Turor/mod-classless](https://github.com/Turor/mod-classless) (`master` to run; work on `develop`, merge to `master`). Full DBC / MPQ / Lua detail is in that README.

**ALE:** the gitlink is a SHA on [Turor/mod-ale](https://github.com/Turor/mod-ale) branch **`classless-acore`** (stock ALE around `a9bc344` plus classless player methods). Do **not** `git submodule update` this core against Turor/mod-ale `master`: that line tracks newer upstream ALE and does not compile here (`OnPlayerResurrect` `bool` vs `bool&`, `ThreatManager`, `WorldPacket const&` packet hooks).

### First-time setup

1. **Clone this repo and the recorded submodules**

```bash
git clone --recurse-submodules https://github.com/Turor/azerothcore-wotlk-classless.git
cd azerothcore-wotlk-classless
git checkout master
git submodule update --init --recursive
```

`apps/consoleport-lk` inside `modules/mod-classless` is a **private** repo. If that submodule fails, skip it; it is only needed to pack ConsolePort into `patch-n.mpq`. Init the public nested modules by hand:

```bash
git -C modules/mod-classless submodule update --init \
  apps/aio apps/transmog apps/teleporter apps/patchgenerator/wow_dbc
```

2. **Build worldserver / authserver** with modules and ALE (LuaJIT) linked in. Out-of-tree CMake; `MODULES=static` is required (classless is compiled into `worldserver`, not a `.so`).

```bash
mkdir -p build && cd build
cmake .. \
  -DCMAKE_INSTALL_PREFIX="$HOME/azeroth-server" \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DSCRIPTS=static \
  -DMODULES=static \
  -DLUA_VERSION=luajit
make -j"$(nproc)" install
```

Turoran wow-dev uses Debug + clang + `WITH_COREDEBUG=true` and the same `MODULES=static` / `LUA_VERSION=luajit` flags. Stock AzerothCore wiki install (MySQL, data, maps, `authserver`/`worldserver` conf) still applies: <https://www.azerothcore.org/wiki/installation>.

3. **Enable classless and ALE**

Copy dist configs into the install tree (names must match; AzerothCore installs `*.conf.dist`):

- `etc/modules/classless.conf` from `modules/mod-classless/conf/classless.conf.dist`  
  Set `ClasslessModule.Enable = 1`. Optional: `ClasslessModule.AchievementTalentPoints`.
- `etc/modules/mod_ale.conf` from `modules/mod-ale/conf/mod_ale.conf.dist`  
  `ALE.Enabled = true`, `ALE.ScriptPath` = your `lua_scripts` directory (absolute path is safest).

World DB SQL under `modules/mod-classless/data/sql/db-world/` (starting skills, spell-train costs, …) is applied by worldserver on startup.

4. **Install Lua scripts** into `ALE.ScriptPath` (default `lua_scripts/` next to the binaries, or the absolute path you set):

| Source in `modules/mod-classless/` | Destination |
|---|---|
| `apps/aio/AIO_Server/` | `lua_scripts/AIO_Server/` |
| `apps/classless-ui/*.lua` | `lua_scripts/ClasslessUI/` |
| `apps/teleporter/TeleportSelector/` | `lua_scripts/TeleportSelector/` |
| `apps/transmog/` (server Lua) | `lua_scripts/Transmogrification/` |

`require("AIO")` only works if `AIO_Server` is on that path. After **client** Lua changes, players need `/aio reset` (or a relog). Lua-only edits do not need a worldserver restart if `ALE.AutoReload` is on; C++, conf, and DBC changes do.

5. **Client patch** (`patch-n.mpq`) — see [mod-classless README](https://github.com/Turor/mod-classless#client-patch-patch-nmpq). You must generate classless DBCs into `UIMods/patch-n/DBFilesClient/` (that folder is gitignored), copy `apps/aio/AIO_Client` into `UIMods/patch-n/Interface/AddOns/AIO_Client`, pack with `mpqcli create -g wow-wotlk`, and copy the same `.dbc` files to the worldserver `data/dbc/` directory. Quit Wow before replacing a live `Data/patch-n.mpq`.

Open the UI in-game with **N** / **P**, `/classless`, or `.classless`.

## Build Status

[![nopch-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core-build-nopch.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core-build-nopch.yml?query=branch%3Amaster)
[![pch-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core-build-pch.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core-build-pch.yml?query=branch%3Amaster)
[![core-modules-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core_modules_build.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/core_modules_build.yml?query=branch%3Amaster)
[![windows-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/windows_build.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/windows_build.yml?query=branch%3Amaster)
[![macos-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/macos_build.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/macos_build.yml?query=branch%3Amaster)
[![docker-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/docker_build.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/docker_build.yml?query=branch%3Amaster)
[![tools-build](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/tools_build.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/tools_build.yml?query=branch%3Amaster)
[![dashboard-ci](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/dashboard-ci.yml/badge.svg?branch=master)](https://github.com/azerothcore/azerothcore-wotlk/actions/workflows/dashboard-ci.yml?query=branch%3Amaster)

## Introduction

AzerothCore is an open-source game server application and framework designed for hosting massively multiplayer online role-playing games (MMORPGs). It is based on the popular MMORPG World of Warcraft (WoW) and seeks to recreate the gameplay experience of the original game from patch 3.3.5a.

The original code is based on MaNGOS, TrinityCore, and SunwellCore and has since then had extensive development to improve stability, in-game mechanics, and modularity to the game. AC has also grown into a community-driven project with a significant number of contributors and developers. It is written in C++ and provides a solid foundation for creating private servers that mimic the mechanics and behavior of the official WoW servers.

## Philosophy

Our main goal is to create a playable game server, offering a fully working in-game experience.

Here are the main points we focus on:

* Stability
  * We make sure all changes pass the CIs before being merged into the master branch.

* Blizzlike content
  * We strive to make all in-game content to be blizzlike. Therefore we have a high standard for fixes being made.

* Customization
  * It is easy to customize your experience using [modules](#modules).

* Community driven
  * AzerothCore has an active community of developers, contributors, and users who collaborate, share knowledge, and provide support through forums, Discord channels, and other communication platforms. 

### Modules

AzerothCore is designed to be highly modular, allowing developers to extend and customize the game to suit their preferences or create unique gameplay experiences. This flexibility enables the addition of custom features, content, and modifications.

We have a lot of modules already made by the community, many of which can be found in the [Module Catalogue](https://www.azerothcore.org/catalogue.html#/).

## Installation

Detailed installation instructions are available [here](http://www.azerothcore.org/wiki/installation).

## Contributing

AzerothCore can also serve as a learning resource for aspiring developers who want to understand how WoW servers work, how MMORPGs are structured, how game server emulators are created, or to improve their C++ and SQL knowledge.

If you want to contribute to the project, you will find a lot of resources that will guide you in our [wiki](https://www.azerothcore.org/wiki/contribute).

We also recommend you read our [Contributor Covenant Code of Conduct](https://github.com/azerothcore/azerothcore-wotlk/blob/master/.github/CODE_OF_CONDUCT.md).

Feel free to join our [Discord server](https://discord.gg/gkt4y2x).

Click on the "⭐ Star" button to help us gain more visibility on Github!

## Authors & Contributors

The project was born in 2016 based on SunwellCore. Unfortunately, SunwellCore was published without any git history, so on git there are no credits for all the contributors before 2016.

You can check the [authors](https://github.com/azerothcore/azerothcore-wotlk/blob/master/AUTHORS) file for more details.

## Important Links

- [Doxygen documentation](https://www.azerothcore.org/pages/doxygen/index.html)
- [Website](http://www.azerothcore.org/)
- [AzerothCore catalogue](http://www.azerothcore.org/catalogue.html  "Modules, tools, and other stuff for AzerothCore") (modules, tools, etc...)
- [Our Discord server](https://discord.gg/gkt4y2x)
- [Our wiki](http://www.azerothcore.org/wiki "Easy to use and developed by AzerothCore founder")
- [Our forum](https://github.com/azerothcore/azerothcore-wotlk/discussions/)
- [Our Facebook page](https://www.facebook.com/AzerothCore/)
- [Our LinkedIn page](https://www.linkedin.com/company/azerothcore/)

## License

- The AzerothCore source code is released under the [GNU GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

It's important to note that AzerothCore is not an official Blizzard Entertainment product, and it is not affiliated with or endorsed by World of Warcraft or Blizzard Entertainment. AzerothCore does not in any case sponsor nor support illegal public servers. If you use this project to run an illegal public server and not for testing and learning it is your own personal choice.

## Special thanks

[JetBrains](https://www.jetbrains.com/?from=AzerothCore) is providing free [open-source licenses](https://www.jetbrains.com/community/opensource/) to the AzerothCore developers.

[![JetBrains logo.](https://resources.jetbrains.com/storage/products/company/brand/logos/jetbrains.svg)](https://jb.gg/OpenSourceSupport)
