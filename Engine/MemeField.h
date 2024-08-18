#pragma once
#include "Graphics.h"
#include "RectI.h"

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
		void Draw(const Vei2& gridPos, const bool GameOver, Graphics& gfx) const;
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
	bool Fucked();
private:
	Tile& TileAt(const Vei2& gridPos);
	const Tile& TileAt(const Vei2& gridPos) const;
	Vei2& ScreenToGrid(Vei2& screenPos) const;
private:
	Vei2 gameOverLoc = { -1, -1 };
	bool isFucked = false;
	static constexpr int width = 20;
	static constexpr int height = 16;
	Tile field[width * height];
};