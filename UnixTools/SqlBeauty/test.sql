select
  aftale.cprnr,
  aftale.navn,
  aftale.angaaende,
  aftale.restraekfoerstetid,
  aftale.restraekfoerstedato,
  '',
  aftale.nepodeastatus,
  aftale.nepostatus,
  aftale.aftaleoid
from aftale
where aftale.afdelingoid = ?
  and aftale.opgaveoid in (
  select
    o.opgaveoid
  from opgaveikategori o
  where o.afdelingoid = ?
    and o.opgavekategorioid = ? )
  and aftale.typehold = 0
  and exists (
  select
    *
  from aftaleressourcekrav ark, nepobegreb nb, nepobegrebressourcegruppe nbrg
  where nb.begrebtype in (
       0, 1 )
    and ark.ressourcegruppeoid = nbrg.ressourcegruppeoid
    and nb.nepobegreboid = nbrg.nepobegreboid
    and ark.aftaleoid = aftale.aftaleoid )
  and ( ( aftale.nepostatus in (
  select
    nepostatusoid
  from nepostatus
  where afdelingoid = aftale.afdelingoid
    and statustype in (
       6, 14 ) )
  and ( ( aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid >= ?
  and aftale.restraekfoerstetid <= ? )
   or ( aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid >= ?
  and aftale.restraekfoerstedato < ? )
   or ( aftale.restraekfoerstedato > ?
  and aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid < ? )
   or ( aftale.restraekfoerstedato > ?
  and aftale.restraekfoerstedato < ? ) )
  and exists (
  select
    *
  from aftaleslot
  where aftaleslot.aftaleoid = aftale.aftaleoid
    and aftaleslot.ressourceoid in (
    select
      ressourceoid
    from ressource
    where ressourcegruppeoid = ?
      and ( closedate is null
       or closedate > sysdate ) ) ) )
   or ( aftale.nepostatus in (
  select
    nepostatusoid
  from nepostatus
  where afdelingoid = aftale.afdelingoid
    and statustype in (
       1, 2 ) )
  and ( ( aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid >= ?
  and aftale.restraekfoerstetid <= ? )
   or ( aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid >= ?
  and aftale.restraekfoerstedato < ? )
   or ( aftale.restraekfoerstedato > ?
  and aftale.restraekfoerstedato = ?
  and aftale.restraekfoerstetid < ? )
   or ( aftale.restraekfoerstedato > ?
  and aftale.restraekfoerstedato < ? ) )
  and exists (
  select
    *
  from aftaleslot
  where aftaleslot.aftaleoid = aftale.aftaleoid
    and aftaleslot.ressourceoid in (
    select
      ressourceoid
    from ressource
    where ressourcegruppeoid = ?
      and ( closedate is null
       or closedate > sysdate ) ) ) ) )
union
 select
  aftale.cprnr,
  aftale.navn,
  aftale.angaaende,
  aftale.restraekfoerstetid,
  aftale.restraekfoerstedato,
  '',
  aftale.nepodeastatus,
  aftale.nepostatus,
  aftale.aftaleoid
from aftale
where aftale.afdelingoid = ?
  and aftale.opgaveoid in (
  select
    o.opgaveoid
  from opgaveikategori o
  where o.afdelingoid = ?
    and o.opgavekategorioid = ? )
  and aftale.typehold = 0
  and aftale.nepostatus in (
  select
    nepostatusoid
  from nepostatus
  where afdelingoid = aftale.afdelingoid
    and statustype in (
       3, 4, 5, 7, 15 ) )
  and exists (
  select
    *
  from aftaleslot
  where aftaleslot.aftaleoid = aftale.aftaleoid
    and aftaleslot.ressourceoid in (
    select
      ressourceoid
    from ressource
    where ressourcegruppeoid = ?
      and ( closedate is null
       or closedate > sysdate ) ) )
