#include "GameOver.h"
#include "System.h"
#include "Graphics.h"
#include "FontEngine.h"
#include "Game.h"

GameOver::GameOver() :
	delay_(0)
{
}

void GameOver::OnActivate(System *system, StateArgumentMap &args)
{
	delay_ = 300;
}

void GameOver::OnUpdate(System *system)
{
	if (--delay_ == 0)
	{
		system->SetNextState("MainMenu");
	}
}

void GameOver::OnRender(System *system)
{
	Graphics *graphics = system->GetGraphics();
	FontEngine *fontEngine = graphics->GetFontEngine();

	system->GetGame()->RenderBackgroundOnly(graphics);

	const char *gameOverText = "Game Over";
	int textWidth = fontEngine->CalculateTextWidth(gameOverText, FontEngine::FONT_TYPE_LARGE);
	int textX = (800 - textWidth) / 2;
	int textY = 200;
	fontEngine->DrawText(gameOverText, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_LARGE);

	int ihighscore = 0, iscore = 0;
	FILE* fp = NULL;
	fp = fopen("score.txt", "r");
	if (fp)
	{
		fscanf(fp, "%d", &ihighscore);
		fscanf(fp, "%d", &iscore);
		fclose(fp);
	}
	std::string scorestr = "High Score: ";
	std::string score = std::to_string(ihighscore);
	std::string result = scorestr + score;

	int textX2 = (300);
	int textY2 = (300);
	fontEngine->DrawText(result, textX2, textY2, 0xff00ffff, FontEngine::FONT_TYPE_MEDIUM);


	scorestr = "Score: ";
	score = std::to_string(iscore);
	result = scorestr + score;
	int textX3 = (300);
	int textY3 = (350);
	fontEngine->DrawText(result, textX3, textY3, 0xff00ffff, FontEngine::FONT_TYPE_MEDIUM);
}

void GameOver::OnDeactivate(System *system)
{
}
