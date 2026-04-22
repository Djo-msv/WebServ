<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <title>Nyan Cat</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }

        body {
            background: #000010;
            overflow: hidden;
            height: 100vh;
            width: 100vw;
            font-family: 'Press Start 2P', monospace;
            cursor: crosshair;
        }

        /* ===== STARFIELD ===== */
        #starfield {
            position: fixed;
            top: 0; left: 0;
            width: 100%; height: 100%;
            z-index: 0;
        }

        /* ===== BOUTON ===== */
        #spawn-btn {
            position: fixed;
            bottom: 40px;
            left: 50%;
            transform: translateX(-50%);
            z-index: 100;
            background: transparent;
            border: 3px solid #ff69b4;
            color: #ff69b4;
            padding: 14px 28px;
            font-family: 'Press Start 2P', monospace;
            font-size: 12px;
            cursor: pointer;
            text-transform: uppercase;
            letter-spacing: 2px;
            box-shadow: 0 0 20px #ff69b4, inset 0 0 10px rgba(255,105,180,0.1);
            transition: all 0.1s;
            image-rendering: pixelated;
            user-select: none;
            -webkit-user-drag: none;
        }

        #spawn-btn:hover {
            background: rgba(255, 105, 180, 0.15);
            box-shadow: 0 0 40px #ff69b4, 0 0 80px #ff69b4, inset 0 0 20px rgba(255,105,180,0.2);
            transform: translateX(-50%) scale(1.05);
        }

        #spawn-btn:active {
            transform: translateX(-50%) scale(0.97);
        }

        /* ===== NYAN CAT ===== */
        .nyan {
            position: fixed;
            z-index: 10;
            display: flex;
            align-items: center;
            pointer-events: auto;
            cursor: pointer;
            user-select: none;
            -webkit-user-drag: none;
        }

        .nyan-sprite {
            width: 200px;
            height: auto;
            image-rendering: pixelated;
            animation: nyan-bob 0.15s steps(1) infinite;
            transition: filter 0.1s;
            user-select: none;
            -webkit-user-drag: none;
            display: block;
        }

        .nyan-sprite:hover {
            filter: brightness(1.3) drop-shadow(0 0 8px #ffff00);
        }

        /* Animation de bobbing si tu as plusieurs frames */
        @keyframes nyan-bob {
            0%, 100% { transform: translateY(0px); }
            50%       { transform: translateY(-4px); }
        }

        /* GIGA NYAN */
        .giga-nyan-container {
            position: fixed;
            z-index: 50;
            top: 50%;
            left: -30vw;
            transform: translateY(-50%);
            display: flex;
            flex-direction: column;
            align-items: center;
            justify-content: center;
            pointer-events: auto;
            gap: 20px;
            user-select: none;
            -webkit-user-drag: none;
        }

        .giga-nyan {
            display: flex;
            align-items: center;
            justify-content: center;
            cursor: pointer;
            position: relative;
            user-select: none;
            -webkit-user-drag: none;
        }

        .giga-nyan-name {
            font-size: 24px;
            color: #ff69b4;
            text-shadow: 0 0 10px #ff69b4, 0 0 20px #ffff00;
            font-weight: bold;
            letter-spacing: 2px;
            margin-bottom: 10px;
            user-select: none;
            -webkit-user-drag: none;
        }

        .giga-nyan-sprite {
            width: 60vw;
            height: auto;
            image-rendering: pixelated;
            animation: giga-bob 0.15s steps(1) infinite;
            filter: drop-shadow(0 0 30px #ff69b4) drop-shadow(0 0 60px #ffff00);
            transition: filter 0.1s;
            user-select: none;
            -webkit-user-drag: none;
            display: block;
        }

        .giga-nyan-sprite:hover {
            filter: brightness(1.3) drop-shadow(0 0 30px #ff69b4) drop-shadow(0 0 60px #ffff00);
        }

        @keyframes giga-bob {
            0%, 100% { transform: translateY(0px); }
            50% { transform: translateY(-8px); }
        }

        /* BARRE DE VIE DARK SOULS */
        .health-bar-container {
            width: 400px;
            height: 40px;
            background: rgba(0, 0, 0, 0.8);
            border: 3px solid #fff;
            position: relative;
            box-shadow: 0 0 20px rgba(255, 105, 180, 0.5);
            user-select: none;
            -webkit-user-drag: none;
        }

        .health-bar-fill {
            height: 100%;
            background: linear-gradient(90deg, #ff0000, #ffff00, #00ff00);
            width: 100%;
            transition: width 0.3s ease;
            position: relative;
            user-select: none;
            -webkit-user-drag: none;
        }

        .health-bar-fill::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent);
            animation: bar-shimmer 0.6s infinite;
        }

        @keyframes bar-shimmer {
            0%, 100% { transform: translateX(-100%); }
            50% { transform: translateX(100%); }
        }

        .health-text {
            position: absolute;
            top: 50%;
            left: 50%;
            transform: translate(-50%, -50%);
            color: #fff;
            font-size: 14px;
            font-weight: bold;
            text-shadow: 0 0 5px rgba(0,0,0,0.8);
            z-index: 10;
            pointer-events: none;
            user-select: none;
            -webkit-user-drag: none;
        }

        /* ===== ÉTOILES SCINTILLANTES ===== */
        .sparkle {
            position: fixed;
            z-index: 5;
            pointer-events: none;
            font-size: 16px;
            animation: sparkle-anim 0.6s ease-out forwards;
            user-select: none;
            -webkit-user-drag: none;
        }

        @keyframes sparkle-anim {
            0%   { opacity: 1; transform: scale(1) rotate(0deg); }
            100% { opacity: 0; transform: scale(0) rotate(180deg); }
        }

        /* ===== EXPLOSION ===== */
        .rainbow-particle {
            position: fixed;
            z-index: 15;
            pointer-events: none;
            border-radius: 50%;
            animation: particle-burst 0.8s ease-out forwards;
            user-select: none;
            -webkit-user-drag: none;
        }

        @keyframes particle-burst {
            0% {
                opacity: 1;
                transform: translate(0, 0) scale(1);
            }
            100% {
                opacity: 0;
                transform: translate(var(--tx), var(--ty)) scale(0);
            }
        }

        /* ===== SONS VISUELS (compteur) ===== */
        #counter {
            position: fixed;
            top: 20px;
            right: 20px;
            color: rgba(255,105,180,0.6);
            font-family: 'Press Start 2P', monospace;
            font-size: 10px;
            z-index: 100;
            user-select: none;
            -webkit-user-drag: none;
        }

        /* Flash d'écran pour événement spécial */
        @keyframes screen-flash {
            0% { opacity: 0; }
            50% { opacity: 0.8; }
            100% { opacity: 0; }
        }

        .screen-flash {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: #ffff00;
            z-index: 200;
            pointer-events: none;
            animation: screen-flash 0.5s ease-out;
            user-select: none;
            -webkit-user-drag: none;
        }
    </style>
    <link href="https://fonts.googleapis.com/css2?family=Press+Start+2P&display=swap" rel="stylesheet">
</head>
<body>

<canvas id="starfield"></canvas>

<div id="counter">NYANS: 0</div>

<button id="spawn-btn" onclick="spawnNyan()">✦ SPAWN NYAN CAT ✦</button>

<!-- Audio elements -->
<audio id="nyan-audio" loop preload="auto">
    <source src="/php/nyancat.mp3" type="audio/mpeg">
</audio>

<audio id="click-sound" preload="auto">
    <source src="/php/minecraft-tnt-explosion.mp3" type="audio/mpeg">
</audio>

<audio id="special-sound" preload="auto">
    <source src="/php/metal-pipe-clang.mp3" type="audio/mpeg">
</audio>

<script>
// =============================================
//   CONFIGURATION — ADAPTE CES VALEURS
// =============================================
const CONFIG = {
    // Chemins vers tes images
    spriteFrames: [
        '/php/nyancat.png',
    ],
    spriteWidth:  200,
    spriteHeight: 120,

    // Vitesse de déplacement (px par frame)
    speed: 5,

    // Nombre max de nyan cats simultanés
    maxNyans: 200,

    // Audio
    audioFadeInDuration: 600,
    audioFadeOutDuration: 600,

    // Probabilité d'événement spécial (1 sur X)
    specialEventChance: 10,

    // Giga nyan
    gigaNyanWidth: 60, // en vw
    gigaNyanHealthMax: 10, // nombre de clics nécessaires
    gigaNyanSpawnCount: 15, // nombre de nyans qui spawn à sa mort
};
// =============================================

let nyanCount = 0;
let activeNyans = 0;
let nyanElements = new Map();
const audioElement = document.getElementById('nyan-audio');
const clickSound = document.getElementById('click-sound');
const specialSound = document.getElementById('special-sound');
let audioFadeInterval = null;
let gigaNyanActive = false;
let gigaNyanHealth = 0;
let gigaNyanMaxHealth = 0;
let gigaNyanAnimationId = null;
let gigaNyanContainer = null;
let gigaNyanX = 0;

// ----- GESTION AUDIO -----
function checkAndUpdateAudio() {
    console.log('Audio check - activeNyans:', activeNyans, 'gigaNyanActive:', gigaNyanActive);
    
    if (activeNyans > 0 && !audioElement.paused) {
        // Audio jouant et des nyans présents - c'est bon
        return;
    }
    
    if (activeNyans > 0 && audioElement.paused) {
        // Des nyans présents mais pas de musique - la démarrer
        console.log('Starting audio - nyans present');
        audioElement.volume = 0;
        audioElement.currentTime = 0;
        audioElement.play().catch(e => console.log('Autoplay bloqué:', e));
        fadeInAudio();
    }
    
    if (activeNyans === 0 && gigaNyanActive === false && !audioElement.paused) {
        // Plus de nyans et giga nyan inactive mais musique jouant - l'arrêter
        console.log('Stopping audio - no nyans present');
        fadeOutAudio();
    }
}

function fadeInAudio() {
    if (audioFadeInterval) clearInterval(audioFadeInterval);
    
    const step = 0.05;
    const steps = 20;
    const interval = CONFIG.audioFadeInDuration / steps;
    let currentStep = 0;

    audioFadeInterval = setInterval(() => {
        currentStep++;
        audioElement.volume = Math.min(1, step * currentStep);
        
        if (currentStep >= steps) {
            audioElement.volume = 1;
            clearInterval(audioFadeInterval);
            audioFadeInterval = null;
        }
    }, interval);
}

function fadeOutAudio() {
    if (audioFadeInterval) clearInterval(audioFadeInterval);
    
    const step = 0.05;
    const steps = 20;
    const interval = CONFIG.audioFadeOutDuration / steps;
    let currentStep = 0;

    audioFadeInterval = setInterval(() => {
        currentStep++;
        audioElement.volume = Math.max(0, 1 - (step * currentStep));
        
        if (currentStep >= steps) {
            audioElement.volume = 0;
            audioElement.pause();
            audioElement.currentTime = 0;
            clearInterval(audioFadeInterval);
            audioFadeInterval = null;
        }
    }, interval);
}

// ----- EXPLOSION ARC-EN-CIEL -----
const RAINBOW_COLORS = ['#ff0000', '#ff7f00', '#ffff00', '#00ff00', '#0000ff', '#4b0082', '#9400d3'];

function createRainbowExplosion(x, y) {
    const particleCount = 30;
    
    for (let i = 0; i < particleCount; i++) {
        const particle = document.createElement('div');
        particle.className = 'rainbow-particle';
        
        const angle = (i / particleCount) * Math.PI * 2;
        const distance = 150 + Math.random() * 100;
        const tx = Math.cos(angle) * distance;
        const ty = Math.sin(angle) * distance;
        
        particle.style.left = x + 'px';
        particle.style.top = y + 'px';
        particle.style.width = '12px';
        particle.style.height = '12px';
        particle.style.backgroundColor = RAINBOW_COLORS[i % RAINBOW_COLORS.length];
        particle.style.setProperty('--tx', tx + 'px');
        particle.style.setProperty('--ty', ty + 'px');
        
        document.body.appendChild(particle);
        setTimeout(() => particle.remove(), 800);
    }
}

// ----- GIGA NYAN -----
function triggerSpecialEvent() {
    gigaNyanActive = true;
    gigaNyanHealth = CONFIG.gigaNyanHealthMax;
    gigaNyanMaxHealth = CONFIG.gigaNyanHealthMax;
    gigaNyanX = -(window.innerWidth * 0.3);

    // Flash l'écran
    const flash = document.createElement('div');
    flash.className = 'screen-flash';
    document.body.appendChild(flash);
    setTimeout(() => flash.remove(), 500);

    // Joue le son spécial
    specialSound.currentTime = 0;
    specialSound.play().catch(e => console.log('Son spécial bloqué:', e));

    // Tue tous les nyans
    nyanElements.forEach((element) => {
        if (element && element.parentNode) {
            element.remove();
        }
    });
    nyanElements.clear();
    activeNyans = 0;

    // Crée le conteneur du giga nyan
    gigaNyanContainer = document.createElement('div');
    gigaNyanContainer.className = 'giga-nyan-container';
    gigaNyanContainer.id = 'giga-nyan-main';
    gigaNyanContainer.style.left = gigaNyanX + 'px';

    // Nom du Giga Nyan
    const gigaNyanName = document.createElement('div');
    gigaNyanName.className = 'giga-nyan-name';
    gigaNyanName.textContent = 'GIGA NYAN';
    gigaNyanName.style.pointerEvents = 'none';

    // Barre de vie
    const healthBarContainer = document.createElement('div');
    healthBarContainer.className = 'health-bar-container';

    const healthBarFill = document.createElement('div');
    healthBarFill.className = 'health-bar-fill';
    healthBarFill.id = 'giga-health-bar';
    healthBarFill.style.width = '100%';

    const healthText = document.createElement('div');
    healthText.className = 'health-text';
    healthText.id = 'giga-health-text';
    healthText.textContent = `${gigaNyanHealth} / ${gigaNyanMaxHealth}`;

    healthBarContainer.appendChild(healthBarFill);
    healthBarContainer.appendChild(healthText);

    // Conteneur du giga nyan
    const gigaContainer = document.createElement('div');
    gigaContainer.className = 'giga-nyan';

    const gigaSprite = document.createElement('img');
    gigaSprite.className = 'giga-nyan-sprite';
    gigaSprite.src = CONFIG.spriteFrames[0];
    gigaSprite.draggable = false;

    // Ajouter l'événement de clic au giga nyan
    gigaSprite.addEventListener('click', (e) => {
        e.stopPropagation();
        handleGigaNyanClick();
    });

    // Aussi sur le conteneur pour ne pas rater
    gigaContainer.addEventListener('click', (e) => {
        e.stopPropagation();
        handleGigaNyanClick();
    });

    function handleGigaNyanClick() {
        if (gigaNyanHealth > 0) {
            gigaNyanHealth--;
            
            // Mise à jour barre de vie
            const healthBar = document.getElementById('giga-health-bar');
            const healthTextEl = document.getElementById('giga-health-text');
            const newWidth = (gigaNyanHealth / gigaNyanMaxHealth) * 100;
            
            if (healthBar) healthBar.style.width = newWidth + '%';
            if (healthTextEl) healthTextEl.textContent = `${gigaNyanHealth} / ${gigaNyanMaxHealth}`;

            // Explosion au clic
            const rect = gigaSprite.getBoundingClientRect();
            const centerX = rect.left + rect.width / 2;
            const centerY = rect.top + rect.height / 2;
            createRainbowExplosion(centerX, centerY);

            clickSound.currentTime = 0;
            clickSound.play().catch(e => console.log('Son clic bloqué:', e));

            if (gigaNyanHealth === 0) {
                destroyGigaNyan();
            }
        }
    }

    gigaContainer.appendChild(gigaSprite);

    gigaNyanContainer.appendChild(gigaNyanName);
    gigaNyanContainer.appendChild(healthBarContainer);
    gigaNyanContainer.appendChild(gigaContainer);
    document.body.appendChild(gigaNyanContainer);

    // Animation du giga nyan (se déplace de gauche à droite)
    function animateGiga() {
        gigaNyanX += CONFIG.speed;
        gigaNyanContainer.style.left = gigaNyanX + 'px';

        if (gigaNyanX < window.innerWidth + (window.innerWidth * 0.3)) {
            gigaNyanAnimationId = requestAnimationFrame(animateGiga);
        } else {
            // Le giga nyan a quitté l'écran naturellement
            console.log('Giga nyan quittant écran naturellement');
            finishGigaNyan();
        }
    }

    gigaNyanAnimationId = requestAnimationFrame(animateGiga);
}

function destroyGigaNyan() {
    // Stop animation
    if (gigaNyanAnimationId) cancelAnimationFrame(gigaNyanAnimationId);

    // Récupère position du centre
    const rect = gigaNyanContainer.getBoundingClientRect();
    const centerX = rect.left + rect.width / 2;
    const centerY = rect.top + rect.height / 2;

    // Grande explosion
    createRainbowExplosion(centerX, centerY);

    // Spawn des nyans à la position
    for (let i = 0; i < CONFIG.gigaNyanSpawnCount; i++) {
        setTimeout(() => {
            if (activeNyans < CONFIG.maxNyans) {
                spawnNyanAtPosition(centerX, centerY);
            }
        }, i * 50);
    }

    // Retire le giga nyan
    finishGigaNyan();
}

function finishGigaNyan() {
    if (gigaNyanContainer && gigaNyanContainer.parentNode) {
        gigaNyanContainer.remove();
    }
    gigaNyanActive = false;
    gigaNyanContainer = null;
    
    console.log('Giga nyan finished - checking audio');
    setTimeout(() => checkAndUpdateAudio(), 50);
}

function spawnNyanAtPosition(x, y) {
    activeNyans++;
    nyanCount++;
    document.getElementById('counter').textContent = `NYANS: ${nyanCount}`;

    console.log('Spawned nyan at position, total active:', activeNyans);
    checkAndUpdateAudio();

    const container = document.createElement('div');
    container.className = 'nyan';
    container.style.userSelect = 'none';

    // Position autour du point d'explosion avec variation
    const randomOffsetX = (Math.random() - 0.5) * 200;
    const randomOffsetY = (Math.random() - 0.5) * 200;
    const yPos = y + randomOffsetY - (CONFIG.spriteHeight / 2);

    container.style.top = Math.max(0, Math.min(window.innerHeight - CONFIG.spriteHeight, yPos)) + 'px';
    container.style.left = x + randomOffsetX + 'px';

    const sprite = document.createElement('img');
    sprite.className = 'nyan-sprite';
    sprite.src = CONFIG.spriteFrames[0];
    sprite.style.width = CONFIG.spriteWidth + 'px';
    sprite.style.height = 'auto';
    sprite.draggable = false;

    if (CONFIG.spriteFrames.length > 1) {
        let frameIdx = 0;
        const frameInterval = setInterval(() => {
            frameIdx = (frameIdx + 1) % CONFIG.spriteFrames.length;
            sprite.src = CONFIG.spriteFrames[frameIdx];
        }, 100);
        container.frameInterval = frameInterval;
    }

    sprite.addEventListener('click', (e) => {
        e.stopPropagation();
        
        const rect = sprite.getBoundingClientRect();
        const centerX = rect.left + rect.width / 2;
        const centerY = rect.top + rect.height / 2;

        createRainbowExplosion(centerX, centerY);

        clickSound.currentTime = 0;
        clickSound.play().catch(e => console.log('Son clic bloqué:', e));

        if (Math.random() < 1 / CONFIG.specialEventChance) {
            triggerSpecialEvent();
        } else {
            container.remove();
            activeNyans--;
            nyanElements.delete(container);
            if (container.frameInterval) clearInterval(container.frameInterval);
            console.log('Nyan removed, remaining active:', activeNyans);
            checkAndUpdateAudio();
        }
    });

    container.appendChild(sprite);
    document.body.appendChild(container);
    nyanElements.set(container, container);

    const initialX = x + randomOffsetX;
    let currentX = initialX;

    function animate() {
        currentX += CONFIG.speed;
        container.style.left = currentX + 'px';

        if (Math.random() < 0.15) {
            spawnSparkle(currentX, parseFloat(container.style.top));
        }

        if (currentX < window.innerWidth + 50) {
            requestAnimationFrame(animate);
        } else {
            if (container.parentNode) container.remove();
            activeNyans--;
            nyanElements.delete(container);
            if (container.frameInterval) clearInterval(container.frameInterval);
            console.log('Nyan left screen, remaining active:', activeNyans);
            checkAndUpdateAudio();
        }
    }

    requestAnimationFrame(animate);
}

// ----- STARFIELD -----
const canvas = document.getElementById('starfield');
const ctx = canvas.getContext('2d');
let stars = [];

function initStars() {
    canvas.width  = window.innerWidth;
    canvas.height = window.innerHeight;
    stars = [];
    for (let i = 0; i < 200; i++) {
        stars.push({
            x:    Math.random() * canvas.width,
            y:    Math.random() * canvas.height,
            r:    Math.random() * 2 + 0.5,
            speed: Math.random() * 0.5 + 0.1,
            opacity: Math.random(),
            twinkleSpeed: Math.random() * 0.02 + 0.005,
            twinkleDir: Math.random() > 0.5 ? 1 : -1,
        });
    }
}

function animateStars() {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    for (const s of stars) {
        s.opacity += s.twinkleSpeed * s.twinkleDir;
        if (s.opacity >= 1)   { s.opacity = 1;   s.twinkleDir = -1; }
        if (s.opacity <= 0.1) { s.opacity = 0.1; s.twinkleDir =  1; }
        ctx.beginPath();
        ctx.arc(s.x, s.y, s.r, 0, Math.PI * 2);
        ctx.fillStyle = `rgba(255,255,255,${s.opacity})`;
        ctx.fill();
    }
    requestAnimationFrame(animateStars);
}

initStars();
animateStars();
window.addEventListener('resize', initStars);

// ----- NYAN CAT -----
function spawnNyan() {
    if (activeNyans >= CONFIG.maxNyans || gigaNyanActive) return;

    activeNyans++;
    nyanCount++;
    document.getElementById('counter').textContent = `NYANS: ${nyanCount}`;

    console.log('Spawned nyan, total active:', activeNyans);
    checkAndUpdateAudio();

    const container = document.createElement('div');
    container.className = 'nyan';
    container.style.userSelect = 'none';

    const yPos = Math.random() * (window.innerHeight - CONFIG.spriteHeight - 20) + 10;
    container.style.top  = yPos + 'px';
    container.style.left = '-' + (CONFIG.spriteWidth + 200) + 'px';

    const sprite = document.createElement('img');
    sprite.className = 'nyan-sprite';
    sprite.src = CONFIG.spriteFrames[0];
    sprite.style.width = CONFIG.spriteWidth + 'px';
    sprite.style.height = 'auto';
    sprite.draggable = false;

    if (CONFIG.spriteFrames.length > 1) {
        let frameIdx = 0;
        const frameInterval = setInterval(() => {
            frameIdx = (frameIdx + 1) % CONFIG.spriteFrames.length;
            sprite.src = CONFIG.spriteFrames[frameIdx];
        }, 100);
        container.frameInterval = frameInterval;
    }

    sprite.addEventListener('click', (e) => {
        e.stopPropagation();
        
        const rect = sprite.getBoundingClientRect();
        const centerX = rect.left + rect.width / 2;
        const centerY = rect.top + rect.height / 2;

        createRainbowExplosion(centerX, centerY);

        clickSound.currentTime = 0;
        clickSound.play().catch(e => console.log('Son clic bloqué:', e));

        if (Math.random() < 1 / CONFIG.specialEventChance) {
            triggerSpecialEvent();
        } else {
            container.remove();
            activeNyans--;
            nyanElements.delete(container);
            if (container.frameInterval) clearInterval(container.frameInterval);
            console.log('Nyan removed, remaining active:', activeNyans);
            checkAndUpdateAudio();
        }
    });

    container.appendChild(sprite);
    document.body.appendChild(container);
    nyanElements.set(container, container);

    let x = -(CONFIG.spriteWidth + 200);

    function animate() {
        x += CONFIG.speed;
        container.style.left = x + 'px';

        if (Math.random() < 0.15) spawnSparkle(x, yPos);

        if (x < window.innerWidth + 50) {
            requestAnimationFrame(animate);
        } else {
            if (container.parentNode) container.remove();
            activeNyans--;
            nyanElements.delete(container);
            if (container.frameInterval) clearInterval(container.frameInterval);
            console.log('Nyan left screen, remaining active:', activeNyans);
            checkAndUpdateAudio();
        }
    }

    requestAnimationFrame(animate);
}

// ----- SPARKLES -----
const SPARKLE_CHARS = ['✦', '✧', '★', '✸', '✺', '✻'];

function spawnSparkle(nyanX, nyanY) {
    const el = document.createElement('div');
    el.className = 'sparkle';
    el.textContent = SPARKLE_CHARS[Math.floor(Math.random() * SPARKLE_CHARS.length)];

    const offsetX = Math.random() * 60 - 30;
    const offsetY = Math.random() * CONFIG.spriteHeight;
    const colors  = ['#ff69b4','#ffff00','#00ffff','#ffffff','#ff99cc'];

    el.style.left  = (nyanX + offsetX) + 'px';
    el.style.top   = (nyanY + offsetY) + 'px';
    el.style.color = colors[Math.floor(Math.random() * colors.length)];

    document.body.appendChild(el);
    setTimeout(() => el.remove(), 600);
}

// ----- CLAVIER -----
document.addEventListener('keydown', (e) => {
    if (e.code === 'Space') {
        e.preventDefault();
        spawnNyan();
    }
});
</script>

</body>
</html>