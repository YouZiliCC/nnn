/*
 * Dark Souls 3 - Open Server
 * Copyright (C) 2021 Tim Leonard
 *
 * This program is free software; licensed under the MIT license.
 * You should have received a copy of the license along with this program.
 * If not, see <https://opensource.org/licenses/MIT>.
 */

#pragma once

#include "Server/GameService/GameManager.h"

struct Frpg2ReliableUdpMessage;
class Server;
class GameService;

// Handles various misc client requests that don't really justify
// having their own manager.

class MiscManager
    : public GameManager
{
public:    
    MiscManager(Server* InServerInstance, GameService* InGameServiceInstance);

    virtual MessageHandleResult OnMessageRecieved(GameClient* Client, const Frpg2ReliableUdpMessage& Message) override;

    virtual std::string GetName() override;

protected:
    MessageHandleResult Handle_RequestNotifyRingBell(GameClient* Client, const Frpg2ReliableUdpMessage& Message);
    MessageHandleResult Handle_RequestSendMessageToPlayers(GameClient* Client, const Frpg2ReliableUdpMessage& Message);
    MessageHandleResult Handle_RequestMeasureUploadBandwidth(GameClient* Client, const Frpg2ReliableUdpMessage& Message);
    MessageHandleResult Handle_RequestMeasureDownloadBandwidth(GameClient* Client, const Frpg2ReliableUdpMessage& Message);
    MessageHandleResult Handle_RequestGetOnlineShopItemList(GameClient* Client, const Frpg2ReliableUdpMessage& Message);
    MessageHandleResult Handle_RequestBenchmarkThroughput(GameClient* Client, const Frpg2ReliableUdpMessage& Message);

private:
    Server* ServerInstance;
    GameService* GameServiceInstance;

};