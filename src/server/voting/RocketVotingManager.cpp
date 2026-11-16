
#include "RocketVotingManager.h"

#include <algorithm>
#include <random>

void RocketVotingManager::reset()
{
    votes.clear();
}

bool RocketVotingManager::submitVote(
    std::uint32_t playerId,
    const std::array<std::uint8_t, 3>& selections)
{
    if (playerId == 0)
        return false;

    // Un jugador solo puede confirmar una vez.
    if (hasVoted(playerId))
        return false;

    // Solo existen las opciones 0, 1 y 2.
    for (auto selection : selections)
    {
        if (selection > 2)
            return false;
    }

    RocketVote vote;

    vote.playerId = playerId;
    vote.selections = selections;
    vote.confirmed = true;

    votes[playerId] = vote;

    return true;
}

bool RocketVotingManager::hasVoted(
    std::uint32_t playerId) const
{
    return votes.find(playerId) != votes.end();
}

bool RocketVotingManager::allPlayersVoted(
    const std::vector<std::uint32_t>& playerIds) const
{
    if (playerIds.empty())
        return false;

    for (auto playerId : playerIds)
    {
        if (!hasVoted(playerId))
            return false;
    }

    return true;
}

std::size_t RocketVotingManager::getVoteCount() const
{
    return votes.size();
}

std::array<std::uint8_t, 3>
RocketVotingManager::calculateWinners() const
{
    std::array<std::uint8_t, 3> winners = { 0, 0, 0 };

    // Generador para resolver empates.
    std::random_device rd;
    std::mt19937 generator(rd());

    // Se calcula cada categoria por separado.
    for (std::size_t category = 0;
        category < 3;
        ++category)
    {
        std::array<int, 3> counts = { 0, 0, 0 };

        for (const auto& [playerId, vote] : votes)
        {
            ++counts[vote.selections[category]];
        }

        int maxVotes = *std::max_element(
            counts.begin(),
            counts.end()
        );

        std::vector<std::uint8_t> tiedOptions;

        for (std::uint8_t option = 0;
            option < 3;
            ++option)
        {
            if (counts[option] == maxVotes)
                tiedOptions.push_back(option);
        }

        // Si hay empate, se elige al azar
        // una de las opciones empatadas.
        std::uniform_int_distribution<std::size_t> dist(
            0,
            tiedOptions.size() - 1
        );

        winners[category] = tiedOptions[
            dist(generator)
        ];
    }

    return winners;
}
