# Étape build : Next build -> /app/out
FROM node:20-alpine AS build
WORKDIR /app
COPY package*.json ./
RUN npm ci
COPY . .
RUN npm run build   # avec output: 'export', ça crée /app/out

# Étape runtime : Caddy sert les fichiers statiques en HTTPS
FROM caddy:2.7-alpine
COPY --from=build /app/out /usr/share/caddy
EXPOSE 80 443
