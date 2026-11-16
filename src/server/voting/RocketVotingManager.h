
#pragma once

#include "game/RocketVote.h"

#include <array>
#include <cstdint>
#include <map>
#include <optional>
#include <vector>

class RocketVotingManager
{
private:
    std::map<std::uint32_t, RocketVote> votes;

public:
    void reset();

    bool submitVote(
        std::uint32_t playerId,
        const std::array<std::uint8_t, 3>& selections
    );

    bool hasVoted(std::uint32_t playerId) const;

    bool allPlayersVoted(
        const std::vector<std::uint32_t>& playerIds
    ) const;

    std::size_t getVoteCount() const;

    std::array<std::uint8_t, 3> calculateWinners() const;
    bool removeVote(std::uint32_t playerId);
};
