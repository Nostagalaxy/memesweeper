#pragma once
#include "Graphics.h"
#include "RectI.h"
#include "Sound.h"

class MemeField
{
	class Tile
	{
	public:
		enum class State
		{
			Hidden,
			Flagged,
			Revealed
		};
	public:
		bool HasMeme() const;
		void SpawnMeme();
		void SetNeighborMemeCount(const int memeCount); //Set the number of neighboring memes for the tile.
		void Draw(const Vei2& screenPos, const bool GameOver, Graphics& gfx) const;
		void Reveal();
		bool IsRevealed() const;
		void ToggleFlag();
		bool IsFlagged();
	private:
		bool isGameOverLoc = false;
		int nNeighborMemes = -1;
		bool hasMeme = false;
		State state = State::Hidden;
	};
public:
	MemeField(const int nMemes);
	void Draw(Graphics& gfx) const;
	void OnClickReveal(Vei2 screenPos);
	void OnClickFlag(Vei2 screenPos);
	RectI& GetRect() const;
	int CountNeighborMemes(const Vei2& gridPos); //Takes a tile location and counts how many memes are neighboring that tile
	bool Fucked() const;
	bool GameWon() const;
private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2& ScreenToGrid(Vei2& screenPos) const;
	Vei2& GridToScreen(Vei2& gridPos) const;
private:
	Sound gameOverSfx;
	Vei2 location;
	Vei2 gameOverLoc = { -1, -1 };
	bool isFucked = false;
	bool gameWon = false;
	int nonMemeTilesLeft;
	static constexpr int width = 6;
	static constexpr int height = 3;
	Tile field[width * height];
};