#include "PlayingState.h"
#include "System.h"
#include "Game.h"
#include <stdio.h>
#include <fstream>
#include <iostream>

PlayingState::PlayingState()
{
}

PlayingState::~PlayingState()
{
}

void PlayingState::OnActivate(System *system, StateArgumentMap &args)
{
	Game *game = system->GetGame();

	level_ = args["Level"].asInt;
	game->InitialiseLevel(level_);
}

void PlayingState::OnUpdate(System *system)
{
	Game *game = system->GetGame();
	game->Update(system);
	if (game->IsLevelComplete())
	{
		StateArgumentMap args;
		args["Level"].asInt = level_ + 1;
		system->SetNextState("LevelStart", args);
	}
	else if (game->IsGameOver())
	{
		int score = game->GetScore();
		int highscore=0;

		std::fstream file;
		FILE* fp = NULL;
		fp = fopen("score.txt", "r");
		if (fp)
		{
			fscanf(fp, "%d", &highscore);
			fclose(fp);
		}

		if (highscore < score)
			highscore = score;

		fp = fopen("score.txt", "w");
		if (fp) {
			fprintf(fp, "%d\n", highscore);
			fprintf(fp, "%d\n", score);
			fclose(fp);
		}
		system->SetNextState("GameOver");
	}
}

void PlayingState::OnRender(System *system)
{
	Game *game = system->GetGame();
	game->RenderEverything(system->GetGraphics());
}

void PlayingState::OnDeactivate(System *system)
{
}
