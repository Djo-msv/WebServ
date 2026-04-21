<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Morpion</title>
  <style>
    @import url('https://fonts.googleapis.com/css2?family=DM+Mono:wght@300;400;500&family=DM+Sans:wght@200;300&display=swap');

    *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

    :root {
      --bg: #0e0e0e;
      --surface: #161616;
      --border: #2a2a2a;
      --text: #e8e8e8;
      --muted: #555;
      --x-color: #e8e8e8;
      --o-color: #888;
      --accent: #3a3a3a;
      --win: #c8f060;
    }

    body {
      background: var(--bg);
      color: var(--text);
      font-family: 'DM Mono', monospace;
      min-height: 100vh;
      display: flex;
      flex-direction: column;
      align-items: center;
      justify-content: center;
      gap: 2.5rem;
    }

    header {
      text-align: center;
      letter-spacing: 0.4em;
      font-size: 0.65rem;
      font-weight: 300;
      color: var(--muted);
      text-transform: uppercase;
    }

    /* Score */
    .score {
      display: flex;
      gap: 3rem;
      align-items: center;
    }

    .score-item {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 0.4rem;
    }

    .score-label {
      font-size: 0.6rem;
      letter-spacing: 0.3em;
      color: var(--muted);
      text-transform: uppercase;
    }

    .score-label.x { color: var(--x-color); }
    .score-label.o { color: var(--o-color); }

    .score-value {
      font-size: 2rem;
      font-weight: 500;
      line-height: 1;
    }

    .score-sep {
      font-size: 0.6rem;
      color: var(--border);
      letter-spacing: 0.2em;
    }

    /* Board */
    .board-wrap {
      position: relative;
    }

    .board {
      display: grid;
      grid-template-columns: repeat(3, 1fr);
      gap: 1px;
      background: var(--border);
      border: 1px solid var(--border);
    }

    .cell {
      width: 110px;
      height: 110px;
      background: var(--surface);
      display: flex;
      align-items: center;
      justify-content: center;
      cursor: pointer;
      transition: background 0.15s;
      position: relative;
    }

    .cell:hover:not(.taken) {
      background: var(--accent);
    }

    .cell.taken { cursor: default; }

    .cell svg {
      width: 44px;
      height: 44px;
      opacity: 0;
      transform: scale(0.6);
      transition: opacity 0.2s, transform 0.2s;
    }

    .cell.show svg {
      opacity: 1;
      transform: scale(1);
    }

    .cell.win-cell {
      background: #1a1f0a;
    }

    .cell.win-cell svg { filter: drop-shadow(0 0 8px var(--win)); }

    /* SVG symbols */
    .sym-x line { stroke: var(--x-color); stroke-width: 2.5; stroke-linecap: round; }
    .sym-o circle { stroke: var(--o-color); stroke-width: 2.5; fill: none; }

    /* Status */
    .status {
      font-size: 0.65rem;
      letter-spacing: 0.35em;
      color: var(--muted);
      text-transform: uppercase;
      min-height: 1.2em;
      text-align: center;
      transition: color 0.3s;
    }

    .status.win { color: var(--win); }
    .status.draw { color: var(--text); }

    /* Reset button */
    .btn-reset {
      background: none;
      border: 1px solid var(--border);
      color: var(--muted);
      font-family: 'DM Mono', monospace;
      font-size: 0.6rem;
      letter-spacing: 0.3em;
      text-transform: uppercase;
      padding: 0.6rem 1.4rem;
      cursor: pointer;
      transition: border-color 0.2s, color 0.2s;
    }

    .btn-reset:hover {
      border-color: var(--text);
      color: var(--text);
    }

    /* Turn indicator */
    .turn {
      display: flex;
      gap: 0.5rem;
      align-items: center;
      font-size: 0.6rem;
      letter-spacing: 0.3em;
      color: var(--muted);
      text-transform: uppercase;
    }

    .turn-dot {
      width: 5px;
      height: 5px;
      border-radius: 50%;
      background: var(--x-color);
      transition: background 0.2s;
    }

    .turn-dot.o { background: var(--o-color); }
  </style>
</head>
<body>

<header>Morpion</header>

<div class="score">
  <div class="score-item">
    <span class="score-label x">×</span>
    <span class="score-value" id="score-x">0</span>
  </div>
  <div class="score-sep">—</div>
  <div class="score-item">
    <span class="score-label o">○</span>
    <span class="score-value" id="score-o">0</span>
  </div>
</div>

<div class="board-wrap">
  <div class="board" id="board">
    <?php for ($i = 0; $i < 9; $i++): ?>
    <div class="cell" data-index="<?= $i ?>">
      <!-- X symbol -->
      <svg class="sym-x" viewBox="0 0 44 44" xmlns="http://www.w3.org/2000/svg" style="display:none">
        <line x1="10" y1="10" x2="34" y2="34"/>
        <line x1="34" y1="10" x2="10" y2="34"/>
      </svg>
      <!-- O symbol -->
      <svg class="sym-o" viewBox="0 0 44 44" xmlns="http://www.w3.org/2000/svg" style="display:none">
        <circle cx="22" cy="22" r="13"/>
      </svg>
    </div>
    <?php endfor; ?>
  </div>
</div>

<div class="turn">
  <div class="turn-dot" id="turn-dot"></div>
  <span id="turn-label">tour de ×</span>
</div>

<div class="status" id="status"></div>

<button class="btn-reset" id="btn-reset">Nouvelle partie</button>

<script>
  const cells = document.querySelectorAll('.cell');
  const statusEl = document.getElementById('status');
  const turnLabel = document.getElementById('turn-label');
  const turnDot = document.getElementById('turn-dot');
  const btnReset = document.getElementById('btn-reset');
  const scoreX = document.getElementById('score-x');
  const scoreO = document.getElementById('score-o');

  let board = Array(9).fill(null);
  let current = 'X';
  let over = false;
  let scores = { X: 0, O: 0 };

  const WINS = [
    [0,1,2],[3,4,5],[6,7,8],
    [0,3,6],[1,4,7],[2,5,8],
    [0,4,8],[2,4,6]
  ];

  function checkWin() {
    for (const [a,b,c] of WINS) {
      if (board[a] && board[a] === board[b] && board[a] === board[c])
        return [a, b, c];
    }
    return null;
  }

  function updateTurn() {
    turnLabel.textContent = `tour de ${current === 'X' ? '×' : '○'}`;
    turnDot.className = 'turn-dot' + (current === 'O' ? ' o' : '');
  }

  function play(idx) {
    if (over || board[idx]) return;
    board[idx] = current;

    const cell = cells[idx];
    const svg = cell.querySelector(current === 'X' ? '.sym-x' : '.sym-o');
    svg.style.display = 'block';
    cell.classList.add('taken');
    requestAnimationFrame(() => cell.classList.add('show'));

    const win = checkWin();
    if (win) {
      over = true;
      win.forEach(i => cells[i].classList.add('win-cell'));
      scores[current]++;
      scoreX.textContent = scores.X;
      scoreO.textContent = scores.O;
      statusEl.textContent = `${current === 'X' ? '×' : '○'} gagne`;
      statusEl.className = 'status win';
      turnLabel.textContent = '—';
      return;
    }

    if (board.every(v => v)) {
      over = true;
      statusEl.textContent = 'égalité';
      statusEl.className = 'status draw';
      turnLabel.textContent = '—';
      return;
    }

    current = current === 'X' ? 'O' : 'X';
    updateTurn();
  }

  cells.forEach(cell => {
    cell.addEventListener('click', () => play(+cell.dataset.index));
  });

  btnReset.addEventListener('click', () => {
    board = Array(9).fill(null);
    current = 'X';
    over = false;
    statusEl.textContent = '';
    statusEl.className = 'status';
    cells.forEach(cell => {
      cell.classList.remove('taken', 'show', 'win-cell');
      cell.querySelectorAll('svg').forEach(s => s.style.display = 'none');
    });
    updateTurn();
  });

  updateTurn();
</script>
</body>
</html>
