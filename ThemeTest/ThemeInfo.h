#ifndef THEMEINFO_H
#define THEMEINFO_H

#include <QString>

enum class ThemeType
{
  Default,
  Blossom,
  Charcoal,
  Adapta,
  AdaptaNokto,
  FTIDark,
  FTILight,
  KvAdapta,
  KvAdaptaDark,
  KvAmbiance,
  KvArc,
  KvArcDark,
  KvGnome,
  KvGnomeAlt,
  KvGnomeDark,
  KvOxygen,
  KvSimplicity,
  KvSimplicityDark,
  Retrosmart
};

// Single source of truth for theme information
struct ThemeInfo
{
  ThemeType type;
  const char* name;
  const char* configPath;
  const char* svgPath;
  const char* colorPath;
};

static const ThemeInfo themeInfos[] = {
  { ThemeType::Default, "Default", ":/Kvantum/default.kvconfig", ":/Kvantum/default.svg", ""},
  { ThemeType::FTIDark, "FTIDark", ":/FTIDark/FTIDark.kvconfig", ":/FTIDark/FTIDark.svg", "" },
  { ThemeType::FTILight, "FTILight", ":/FTILight/FTILight.kvconfig", ":/FTILight/FTILight.svg", "" },
  { ThemeType::Blossom, "Blossom", ":/Blossom/Blossom.kvconfig", ":/Blossom/Blossom.svg", "" },
  { ThemeType::Charcoal, "Charcoal", ":/Charcoal/Charcoal.kvconfig", ":/Charcoal/Charcoal.svg", "" },
  { ThemeType::Adapta, "Adapta", ":/Adapta/Adapta.kvconfig", ":/Adapta/Adapta.svg", "" },
  { ThemeType::AdaptaNokto, "AdaptaNokto", ":/AdaptaNokto/AdaptaNokto.kvconfig", ":/AdaptaNokto/AdaptaNokto.svg", "" },
  { ThemeType::KvAdapta, "KvAdapta", ":/KvAdapta/KvAdapta.kvconfig", ":/KvAdapta/KvAdapta.svg", "" },
  { ThemeType::KvAdaptaDark, "KvAdaptaDark", ":/KvAdaptaDark/KvAdaptaDark.kvconfig", ":/KvAdaptaDark/KvAdaptaDark.svg", "" },
  { ThemeType::KvAmbiance, "Ambiance", ":/KvAmbiance/KvAmbiance.kvconfig", ":/KvAmbiance/KvAmbiance.svg", "" },
  { ThemeType::KvArc, "Arc", ":/KvArc/KvArc.kvconfig", ":/KvArc/KvArc.svg", "" },
  { ThemeType::KvArcDark, "ArcDark", ":/KvArcDark/KvArcDark.kvconfig", ":/KvArcDark/KvArcDark.svg", "" },
  { ThemeType::KvGnome, "Gnome", ":/KvGnome/KvGnome.kvconfig", ":/KvGnome/KvGnome.svg", "" },
  { ThemeType::KvGnomeAlt, "GnomeAlt", ":/KvGnomeAlt/KvGnomeAlt.kvconfig", ":/KvGnomeAlt/KvGnomeAlt.svg", "" },
  { ThemeType::KvGnomeDark, "GnomeDark", ":/KvGnomeDark/KvGnomeDark.kvconfig", ":/KvGnomeDark/KvGnomeDark.svg", "" },
  { ThemeType::KvOxygen, "Oxygen", ":/KvOxygen/KvOxygen.kvconfig", ":/KvOxygen/KvOxygen.svg", "" },
  { ThemeType::KvSimplicity, "Simplicity", ":/KvSimplicity/KvSimplicity.kvconfig", ":/KvSimplicity/KvSimplicity.svg", "" },
  { ThemeType::KvSimplicityDark, "SimplicityDark", ":/KvSimplicityDark/KvSimplicityDark.kvconfig", ":/KvSimplicityDark/KvSimplicityDark.svg", "" },
  { ThemeType::Retrosmart, "Retrosmart", ":/Retrosmart/Retrosmart.kvconfig", ":/Retrosmart/Retrosmart.svg", "" }
};

// Helper to get theme info by type
static const ThemeInfo* getThemeInfo(ThemeType type)
{
  for (const auto& info : themeInfos)
  {
    if (info.type == type)
      return &info;
  }
  return &themeInfos[0]; // Default
}

// Helper to get theme info by name (case-insensitive)
static const ThemeInfo* getThemeInfo(const QString& name)
{
  for (const auto& info : themeInfos)
  {
    if (name.compare(info.name, Qt::CaseInsensitive) == 0)
      return &info;
  }
  return &themeInfos[0]; // Default
}

#endif // THEMEINFO_H